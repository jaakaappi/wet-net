const { default: axios } = require("axios");
const express = require("express");
require("dotenv").config();
_ = require("lodash");

const app = express();
const port = 8081;

const deviceIdPairs = [
  { label: "kitchen", id: process.env.KITCHEN_ID },
  { label: "bathroom", id: process.env.BATHROOM_ID },
];

const generateAxiosRequest = (device_id, token) => {
  return {
    method: "get",
    url: `http://${
      process.env.THINGSBOARD_URL
    }/api/plugins/telemetry/DEVICE/${device_id}/values/timeseries?useStrictDataTypes=false&keys=temperature,humidity&startTs=${
      Date.now() - 48 * 60 * 60 * 1000
    }&endTs=${Date.now()}&limit=1000000`,
    headers: {
      "content-type": "application/json",
      "X-Authorization": `Bearer ${token}`,
    },
  };
};

app.get("/api", async (req, res) => {
  try {
    const auth_response = await axios({
      method: "post",
      url: `http://${process.env.THINGSBOARD_URL}/api/auth/login`,
      data: {
        username: process.env.THINGSBOARD_USERNAME,
        password: process.env.THINGSBOARD_PASSWORD,
      },
      headers: {
        "content-type": "application/json",
      },
    });
    const token = auth_response.data.token;

    const responseData = await Promise.all(
      deviceIdPairs.map(async (value) => {
        const deviceResponseData = (
          await axios(generateAxiosRequest(value.id, token))
        ).data;
        const sortedData = deviceResponseData.humidity.map((value, index) => {
          return [
            value.ts,
            value.value,
            deviceResponseData.temperature[index].value,
          ];
        });
        const filteredData = sortedData.filter(
          (data) =>
            data[1] <= 100 && data[1] >= 0 && data[2] <= 100 && data[2] >= -100
        );
        const unzippedData = _.unzip(filteredData);
        return {
          name: value.label,
          timestamps: unzippedData[0],
          humidity: unzippedData[1],
          temperature: unzippedData[2],
        };
      })
    );

    res.send(responseData);
  } catch (error) {
    console.error(error);
    res.status(500).send(error);
  }
});

app.use(express.static("public"));

app.listen(port);

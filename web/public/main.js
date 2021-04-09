const generatePage = (devices) => {
  console.log(devices);
  const container = document.getElementById("container");

  devices.forEach((device, i) => {
    const chartContainer = document.createElement("div");
    chartContainer.classList.add("chartContainer");
    chartContainer.id = device.name;
    chartContainer.style.width = "775px";

    container.appendChild(chartContainer);

    var chart = c3.generate({
      title: {
        text: device.name,
      },
      size: {
        height: 220,
        width: 775,
      },
      bindto: `#${device.name}`,
      data: {
        x: "x",
        columns: [
          ["x", ...device.timestamps],
          ["humidity", ...device.humidity],
          ["temperature", ...device.temperature],
        ],
        axes: {
          humidity: "y2",
        },
      },
      point: {
        show: false,
      },
      axis: {
        x: {
          type: "timeseries",
          tick: {
            count: 12,
            format: "%H:%m",
          },
        },
        y: {
          label: {
            text: "temperature",
            position: "outer-middle",
          },
          position: "outer-middle",
        },
        y2: {
          show: true,
          label: {
            text: "humidity",
            position: "outer-middle",
          },
        },
      },
    });
  });
};

const showError = (err) => {
  console.error(err);
  const container = document.getElementById("container");
  const errorText = document.createElement("p");
  errorText.innerHTML = `There was an error fetching data from the API: ${err.message}`;
};

fetch("http://localhost:8081/api")
  .then((response) => response.json())
  .then((json) => generatePage(json))
  .catch((err) => showError(err));

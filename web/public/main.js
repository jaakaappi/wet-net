let charts = {};
let updateTimestamp = 0;

const generatePage = (devices) => {
  console.log(new Date().toLocaleString());
  const container = document.getElementById("container");
  container.innerHTML = "";

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
          label: {
            text: "time of day",
            position: "outer-center",
          },
          tick: {
            count: 12,
            format: "%H:%m",
          },
        },
        y: {
          label: {
            text: "temperature °C",
            position: "outer-middle",
          },
          position: "outer-middle",
        },
        y2: {
          show: true,
          label: {
            text: "humidity rel-%",
            position: "outer-middle",
          },
        },
      },
    });
    charts[device.name] = chart;
  });
};

const updatePage = (devices) => {
  devices.forEach((device) => {
    const chart = charts[device.name];

    chart.load({
      columns: [
        ["x", ...device.timestamps],
        ["temperature", ...device.temperature],
        ["humidity", ...device.humidity],
      ],
    });
  });
};

const showError = (err) => {
  console.error(err);
  const container = document.getElementById("container");
  const errorText = document.createElement("p");
  errorText.innerHTML = `There was an error fetching data from the API: ${err.message}`;
  container.appendChild(errorText);
};

fetch(`http://localhost:8081/api?from=${Date.now() - 48 * 60 * 60 * 1000}`)
  .then((response) => response.json())
  .then((json) => generatePage(json))
  .catch((err) => showError(err));

setInterval(() => {
  fetch(`http://localhost:8081/api?from=${Date.now() - 48 * 60 * 60 * 1000}`)
    .then((response) => response.json())
    .then((json) => updatePage(json))
    .catch((err) => showError(err));
}, 10 * 60 * 1000);

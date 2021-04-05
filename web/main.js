const devices = [
  {
    name: "iot1",
    humidity: [1, 2, 1, 4, 5],
    temperature: [2, 7, 6, 4, 3],
    timestamps: [1, 2, 3, 4, 5],
  },
  {
    name: "iot2",
    humidity: [1, 2, 1, 4, 5],
    temperature: [2, 7, 6, 4, 3],
    timestamps: [1, 2, 3, 4, 5],
  },
  {
    name: "iot3",
    humidity: [1, 2, 1, 4, 5],
    temperature: [2, 7, 6, 4, 3],
    timestamps: [1, 2, 3, 4, 5],
  },
];

const container = document.getElementById("container");

devices.forEach((device, i) => {
  const chartContainer = document.createElement("div");
  chartContainer.classList.add("chartContainer");
  chartContainer.id = device.name;
  chartContainer.style.width = "775px";
  if (i % 2 == 1) chartContainer.style.backgroundColor = "#dddddd";

  container.appendChild(chartContainer);

  var chart = c3.generate({
    title: {
      text: device.name,
    },
    size: {
      height: 150,
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
    axis: {
      y2: {
        show: true,
      },
    },
  });
});

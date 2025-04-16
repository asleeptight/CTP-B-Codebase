let chartInstance = null; // optional to prevent duplicate charts

function renderChart(data) {
  const ctx = document.getElementById('activityChart').getContext('2d');

  if (chartInstance) {
    chartInstance.destroy(); // clears previous chart if it exists
  }

  chartInstance = new Chart(ctx, {
    type: 'bar', // chart type (can be 'line', 'pie', etc.)
    data: {
      labels: Object.keys(data), // x-axis labels (dates)
      datasets: [{
        label: 'Interactions Per Day', // chart title
        data: Object.values(data),     // y-axis values (number of touches)
        backgroundColor: 'rgba(52, 152, 219, 0.6)', // bar color
        borderRadius: 5
      }]
    },
    options: {
      responsive: true,
      scales: {
        y: {
          beginAtZero: true,
          ticks: {
            stepSize: 1
          }
        }
      },
      plugins: {
        legend: {
          labels: {
            color: '#2c3e50'
          }
        }
      }
    }
  });
}

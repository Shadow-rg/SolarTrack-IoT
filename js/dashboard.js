// ==========================
// AUTENTICACIÓN
// ==========================
if(localStorage.getItem("solartrackAuth") !== "true"){
    window.location.href = "index.html";
}

// Cerrar sesión
function logout(){
    localStorage.removeItem("solartrackAuth");
    window.location.href = "index.html";
}

// ==========================
// HISTORIAL DINÁMICO
// ==========================
let historicalData = []; // Se llena con datos recibidos vía MQTT
const historyTable = document.getElementById("historyTable");

// Renderizar tabla
function renderTable(dataArray){
    historyTable.innerHTML = "";
    dataArray.forEach(data => {
        let statusColor = "text-green-400";
        if(data.status === "OFFLINE") statusColor = "text-red-400";
        if(data.status === "BATTERY MODE") statusColor = "text-yellow-400";

        historyTable.innerHTML += `
            <tr class="border-b border-white/5 hover:bg-white/5">
                <td class="p-4">${data.date}</td>
                <td class="p-4">${data.solar} W/m²</td>
                <td class="p-4">${data.current} A</td>
                <td class="p-4">${data.battery}%</td>
                <td class="p-4">${data.servoH}°</td>
                <td class="p-4">${data.servoV}°</td>
                <td class="p-4">${data.efficiency}%</td>
                <td class="p-4 ${statusColor}">${data.status}</td>
            </tr>
        `;
    });
}

// ==========================
// MÉTRICAS
// ==========================
function updateMetricsRealtime(payload){
    document.getElementById("solarMetric").innerText = payload.solar;
    document.getElementById("currentMetric").innerText = payload.current + "A";
    document.getElementById("batteryMetric").innerText = payload.battery + "%";
    document.getElementById("servoHMetric").innerText = payload.servoH + "°";
    document.getElementById("servoVMetric").innerText = payload.servoV + "°";
    document.getElementById("efficiencyMetric").innerText = payload.efficiency + "%";
    document.getElementById("lastSync").innerText = new Date().toLocaleString();
}

// ==========================
// GRÁFICAS
// ==========================
const energyChart = new Chart(document.getElementById("energyChart"), {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Producción Energética (W/m²)',
            data: [],
            borderColor: 'yellow',
            fill: false
        }]
    }
});

const compareChart = new Chart(document.getElementById("compareChart"), {
    type: 'line',
    data: {
        labels: [],
        datasets: [
            {
                label: 'Tracker',
                data: [],
                borderColor: 'cyan',
                fill: false
            },
            {
                label: 'Panel Fijo',
                data: [],
                borderColor: 'purple',
                fill: false
            }
        ]
    }
});

function updateCharts(payload){
    const time = new Date().toLocaleTimeString();
    energyChart.data.labels.push(time);
    energyChart.data.datasets[0].data.push(payload.solar);
    energyChart.update();

    compareChart.data.labels.push(time);
    compareChart.data.datasets[0].data.push(payload.efficiency);
    compareChart.data.datasets[1].data.push(payload.efficiency - 15); // Simulación panel fijo
    compareChart.update();
}

// ==========================
// EXPORTAR CSV
// ==========================
function exportCSV(){
    let csv = "Fecha,Solar,Corriente,Batería,ServoH,ServoV,Eficiencia,Estado\n";
    historicalData.forEach(d => {
        csv += `${d.date},${d.solar},${d.current},${d.battery},${d.servoH},${d.servoV},${d.efficiency},${d.status}\n`;
    });
    const blob = new Blob([csv], { type: "text/csv" });
    const url = window.URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "solartrack_history.csv";
    a.click();
}

// ==========================
// FILTRO DE HISTORIAL
// ==========================
function filterData(){
    const filter = document.getElementById("syncFilter").value;
    if(filter === "ALL"){
        renderTable(historicalData);
        return;
    }
    const filtered = historicalData.filter(data => data.sync === filter);
    renderTable(filtered);
}

// ==========================
// MQTT CLIENT
// ==========================
const client = new Paho.MQTT.Client("broker.hivemq.com", 8000, "SolarTrackWeb");

client.connect({onSuccess:onConnect});

function onConnect() {
    console.log("Conectado al broker MQTT");
    client.subscribe("solartrack/data");
}

// Al recibir mensaje MQTT
client.onMessageArrived = function(message) {
    const payload = JSON.parse(message.payloadString);

    const newData = {
        sync:"ULTIMA",
        date:new Date().toLocaleString(),
        solar:payload.solar,
        current:payload.current,
        battery:payload.battery,
        servoH:payload.servoH,
        servoV:payload.servoV,
        efficiency:payload.efficiency,
        status:"ONLINE"
    };

    historicalData.push(newData);
    renderTable(historicalData);
    updateMetricsRealtime(payload);
    updateCharts(payload);
};

// ==========================
// DESCARGAR GRÁFICAS COMO PNG
// ==========================
function downloadChart(chartId){
    html2canvas(document.getElementById(chartId)).then(canvas => {
        const link = document.createElement("a");
        link.download = chartId + ".png";
        link.href = canvas.toDataURL();
        link.click();
    });
}

// ==========================
// MODO DE VISTA (PROMEDIOS / ÚLTIMA)
// ==========================
let averageMode = true;

function toggleViewMode(){
    averageMode = !averageMode;
    if(averageMode){
        document.getElementById("viewModeBtn").innerText = "MODO: PROMEDIOS";
    } else {
        document.getElementById("viewModeBtn").innerText = "MODO: ÚLTIMA SINCRONIZACIÓN";
    }
}
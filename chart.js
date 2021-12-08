/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 04-11-2021 21:26:05
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 19-11-2021 14:10:14
 * @ Description: AiR 2021/2022, grupa: 7
 */

/*!
 * Chart.js v3.4.1
 * https://www.chartjs.org
 * (c) 2021 Chart.js Contributors
 * Released under the MIT License
 */

const ctx = document.getElementById('myChart').getContext('2d');
const rtx = document.getElementById('myChartH').getContext('2d');

cahrtIt1();
cahrtIt2();

async function getData() {
    const xs = [];
    const ysT = [];
    const ysH = [];
    const response = await fetch('temp.csv');
    const data = await response.text();
    //console.log(data);

    const table = data.split('\n').slice(2);
    table.forEach(row => {
        const columns = row.split(',')
        // console.log(columns);
        const time = odliczanie1(columns[0]*1000);
        xs.push(time);
        const temp = columns[1];
        ysT.push(temp);
        const hum = columns[2];
        ysH.push(hum);
    })
    return {xs,ysT,ysH}
}

async function cahrtIt1(){
    const data = await getData();
    // console.log(data);
    const myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: data.xs,
            datasets: [{
                label: 'Temperatura',
                data: data.ysT,
                fill: false,
                scaleFontColor: "#FFFFFF"
            }]
        },
        options: {

            // TODO: WYKRES NA CAŁEGO DIVA
            maintainAspectRatio: false,

            elements: {
                line: {
                    borderColor: 'Black',
                    backgroundColor: 'Black',
                    borderWidth: 1,
                    lineTension: 0.2
                },
                point:{
                    fill: false,
                    borderColor: 'Blue',
                    borderWidth: .5,
                    hitRadius: 20,
                    radius: 1,
                    pointStyle: 'circle',
                    hoverRadius: 20,
                    hoverBorderWidth: 10
                }
            },
            scales: {
                x: {
                    ticks: {
                        color: 'red',
                        callback: function(value){
                            const valueLegend = this.getLabelForValue(value);
                            const valueLegendRep = valueLegend.replaceAll(':',':');
                            const valueLegendTruc = valueLegendRep.substr(11,5);
                            return valueLegendTruc;
                        }
                    },
                    grid: {
                        color: 'gray'
                    }
                },
                y: {
                    beginAtZero: false
                },
                y: {
                    ticks: {
                        // Include a dollar sign in the ticks
                        callback: function(value, index, values) {
                            return value + "°C";
                        },
                        color: 'red'
                    },
                    grid: {
                        color: 'gray'
                    }
                }
            },
            events: ['mousemove', 'mouseout', 'click', 'touchstart', 'touchmove'],
            plugins: {
                tooltip: {

                            // Tooltip will only receive click events
                    events: ['click','mausemove' ]
                }
            }

        }
    });
}

async function cahrtIt2(){
    const data = await getData();
    // console.log(data);
    const myChart = new Chart(rtx, {
        type: 'line',
        data: {
            labels: data.xs,
            datasets: [{
                label: 'Wilgotność',
                data: data.ysH,
                // backgroundColor: [
                    //     'rgba(0, 0, 0)'
                    // ],
                    // borderColor: [
                        //     'rgba(0, 0, 0)',
                        // ],
                        fill: false,
                scaleFontColor: "#FFFFFF"
            }]
            ,
        },
        options: {

            // TODO: WYKRES NA CAŁEGO DIVA
            maintainAspectRatio: false,

            elements: {
                line: {
                    borderColor: 'Black',
                    backgroundColor: 'Black',
                    borderWidth: 1,
                    lineTension: 0.2
                },
                point:{
                    fill: false,
                    borderColor: 'Blue',
                    borderWidth: .5,
                    hitRadius: 20,
                    radius: 1,
                    pointStyle: 'circle',
                    hoverRadius: 20,
                    hoverBorderWidth: 10
                }
            },
            scales: {
                x: {
                    ticks: {
                        color: 'red',
                        callback: function(value){
                            const valueLegend = this.getLabelForValue(value);
                            const valueLegendRep = valueLegend.replaceAll(':',':');
                            const valueLegendTruc = valueLegendRep.substr(11,5);
                            return valueLegendTruc;
                        }
                    },
                    grid: {
                        color: 'gray'
                    }
                },
                y: {
                    beginAtZero: false
                },
                y: {
                    ticks: {
                        // Include a dollar sign in the ticks
                        callback: function(value, index, values) {
                            return value + '%';
                        },
                        color: 'red'
                    },
                    grid: {
                        color: 'gray'
                    }
                }
            },
            events: ['mousemove', 'mouseout', 'click', 'touchstart', 'touchmove'],
            plugins: {
                tooltip: {

                            // Tooltip will only receive click events
                    events: ['click','mausemove' ]
                }
            }

        }
    });
}

function odliczanie1(dzis) {

    // console.log(dzis);
    var dzisiaj = new Date(dzis);

    var dzien = dzisiaj.getDate();
    var miesiac = dzisiaj.getMonth() + 1; // ponieważ liczy od 0 - 11
    var rok = dzisiaj.getFullYear();

    var godzina = dzisiaj.getHours();
    var minuta = dzisiaj.getMinutes();
    var sekunda = dzisiaj.getSeconds();

    if(godzina < 10) godzina = "0" + godzina;
    if(minuta < 10) minuta = "0" + minuta;
    if(sekunda < 10) sekunda = "0" + sekunda;
    if(dzien < 10) dzien = "0" + dzien;
    if(miesiac < 10) miesiac = "0" + miesiac;

    // console.log(dzien+'/'+miesiac+'/'+rok+' '+godzina+':'+minuta+':'+sekunda);
    return dzien+'/'+miesiac+'/'+rok+' '+godzina+':'+minuta+':'+sekunda;
}
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
        console.log(columns);
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
    console.log(data);
    const myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: data.xs,
            datasets: [{
                label: 'Temperatura',
                data: data.ysT,
                fill: false,
                backgroundColor: [
                    'rgba(0,0, 0)'
                ],
                borderColor: [
                    'rgba(0, 0, 0)',
                ],
                borderWidth: 1,
                scaleFontColor: "#FFFFFF"
                ,
            //    Fontcolor: [
              //      'rgba(255, 255, 255)',
              //  ],
             //   options:{
               //     scales:{
                 //       x:{
                   //         type: 'time',
                     //       time: {
                               // unit: 'second' // format danych w sekundach NIE DZIAŁA //
                       //      displayFormats:
                         //    {
                           //     quarter: 'MMM YYYY' // format daty NIE DZIAŁA
                          //   }
                               
                     //   }
                     //   }
                  //  }
              //  }
                
            }]
        },
        options: {

            elements: {
                line: {
                    lineTension: 0.2
                },
                point:{
                    radius: 1.5
                }
            },
            scales: {
                x: {
                    ticks: {
                        color: 'red'
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
                            return value + '°C';
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
    console.log(data);
    const myChart = new Chart(rtx, {
        type: 'line',
        data: {
            labels: data.xs,
            datasets: [{
                label: 'Wilgotność',
                data: data.ysH,
                fill: false,
                backgroundColor: [
                    'rgba(0, 0, 0)'
                ],
                borderColor: [
                    'rgba(0, 0, 0)',
                ],
                borderWidth: 1,
                scaleFontColor: "#FFFFFF"
            }]
            ,
          //  Fontcolor: [
            //    'rgba(255, 255, 255)',
           // ],
       //  options:{
           //     scales:{
             //       x:{
               //         type: 'time',
                 //       time: {
                   //         unit: 'seconds'
                      //   displayFormats:
                       // {
                        //    quarter: 'MMM YYYY'
                        // }
                           
           //             }
            //        }
            //    }
         //  }
        },
        options: {

            elements: {
                line: {
                    lineTension: 0.2
                },
                point:{
                    radius: 1.5
                }
            },
            scales: {
                x: {
                    ticks: {
                        color: 'red'
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

    console.log(dzis);
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

    console.log(dzien+'/'+miesiac+'/'+rok+' '+godzina+':'+minuta+':'+sekunda);

    return dzien+'/'+miesiac+'/'+rok+' '+godzina+':'+minuta+':'+sekunda;
}
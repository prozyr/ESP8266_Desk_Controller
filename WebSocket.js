/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 04-11-2021 21:26:05
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 19-11-2021 14:10:50
 * @ Description: AiR 2021/2022, grupa: 7
 */

var pirButtonEnable = false;
var rainbowEnable = false;
var buttonK1Enalbe = false;
var connection = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);

connection.onopen = function () {
    connection.send('Connect ' + new Date());
};

connection.onerror = function (error) {
    console.log('WebSocket Error ', error);
};

connection.onmessage = function (e) {
    console.log('Server: ', e.data);

    var tmp = e.data.split(',');

    if (tmp[0] == "!") { // When message start with "!", its data of state PWM modulation of RGB light, 0 - 255. Data format !,Red,Green,Blue
        document.getElementById("r").value = tmp[1];
        document.getElementById("g").value = tmp[2];
        document.getElementById("b").value = tmp[3];
        document.getElementById("klocekRGB").style.backgroundColor = 'rgb('+ tmp[1] + ',' + tmp[2] + ',' + tmp[3] + ')';
        document.getElementById("output").innerHTML = 'rgb('+ tmp[1] + ',' + tmp[2] + ',' + tmp[3] + ')';
    }
};

connection.onclose = function(){
    console.log('WebSocket connection closed');
};

function sendRGB() {
    colors();
    var r = document.getElementById('r').value;
    var g = document.getElementById('g').value;
    var b = document.getElementById('b').value;

    var rgb = r << 20 | g << 10 | b;
    var rgbstr = '#'+ rgb.toString(16);
    console.log('RGB: ' + rgbstr);
    connection.send(rgbstr);
}

function rainbowEffect(){
    rainbowEnable = ! rainbowEnable;
    if(rainbowEnable){
        connection.send("R");
        document.getElementById('rainbow').style.backgroundColor = '#00878F';
        document.getElementById('r').className = 'disabled';
        document.getElementById('g').className = 'disabled';
        document.getElementById('b').className = 'disabled';
        document.getElementById('r').disabled = true;
        document.getElementById('g').disabled = true;
        document.getElementById('b').disabled = true;
    } else {
        connection.send("N");
        document.getElementById('rainbow').style.backgroundColor = '#999';
        document.getElementById('r').className = 'enabled';
        document.getElementById('g').className = 'enabled';
        document.getElementById('b').className = 'enabled';
        document.getElementById('r').disabled = false;
        document.getElementById('g').disabled = false;
        document.getElementById('b').disabled = false;
        sendRGB();
    }
}

// STREFA ZAŁĄCZANIA PRZEKAŹNIKÓW
function buttonPIR(){
    connection.send("p");
}

function buttonK1(){
    connection.send("1");
}

function buttonK2(){
    connection.send("2");
}

function buttonK3(){
    connection.send("3");
}

function buttonK4(){
    connection.send("4");
}

function buttonDel(){
    connection.send("D");
}

function reset(){
    connection.send("r");
}
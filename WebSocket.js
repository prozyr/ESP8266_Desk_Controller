/**
 * @ Author: Maciej Żyrek
 * @ Create Time: 09.06.2021 12:45:06
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 04-11-2021 19:16:42
 * @ Description:
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
    document.getElementById('Dt').innerHTML = tmp[0];

    document.getElementById('Dt').style.color = tmp[0] <= 0 ? 'red' : 'green';

    document.getElementById('Dh').innerHTML = tmp[1];

    document.getElementById('D').style.color = tmp[0] <= 0 ? 'red' : 'green';
};
connection.onclose = function(){
    console.log('WebSocket connection closed');
};

function sendRGB() {
    colors();
    var r = document.getElementById('r').value**2/1023;
    var g = document.getElementById('g').value**2/1023;
    var b = document.getElementById('b').value**2/1023;

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

// TODO: zakomentowane, wykresy są wywoływane przez przyciski na stronie ... do poprawy
/*
    setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("przycisk2").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "temperature", true);
    xhttp.send();
    }, 10000 ) ;

setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
        document.getElementById("przycisk1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "humidity", true);
  xhttp.send();
  }, 10000 ) ;
  */

  window.onload = function(){
    var button = document.getElementById('przycisk1');
    var button1 = document.getElementById('przycisk2');
    button.click();
    button1.click();

    connection.send("X");

    setInterval(function(){

        connection.send("X");
        button.click();
        button1.click();
    },60000);  // this will make it click again every 1000 miliseconds
};
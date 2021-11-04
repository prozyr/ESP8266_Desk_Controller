/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 04-11-2021 18:06:05
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 04-11-2021 19:18:04
 * @ Description: AiR 2021/2022, grupa: 7
 * 
 * Zmienia kolor klocka
 */

colors();

function colors()
{   
    var red = parseInt((document.getElementById("r").value)/4);
    var green = parseInt((document.getElementById("g").value)/4);
    var blue = parseInt((document.getElementById("b").value)/4);
    document.getElementById("klocekRGB").style.backgroundColor = 'rgb('+ red + ',' + green + ',' + blue + ')';
    document.getElementById("output").innerHTML = 'rgb('+ red + ',' + green + ',' + blue + ')';

    //var rgb = r << 20 | g << 10 | b;
    //var rgbstr = '#'+ rgb.toString(16);
    //console.log('RGB: ' + rgbstr);
    //connection.send(rgbstr);
}
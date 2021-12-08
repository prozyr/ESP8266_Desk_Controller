/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 04-11-2021 18:06:05
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 19-11-2021 14:11:44
 * @ Description: AiR 2021/2022, grupa: 7
 *
 * Zmienia kolor klocka
 */

colors();

function colors()
{
    var red = parseInt((document.getElementById("r").value));
    var green = parseInt((document.getElementById("g").value));
    var blue = parseInt((document.getElementById("b").value));
    document.getElementById("klocekRGB").style.backgroundColor = 'rgb('+ red + ',' + green + ',' + blue + ')';
    document.getElementById("output").innerHTML = 'rgb('+ red + ',' + green + ',' + blue + ')';
}
/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 18.10.2021 14:39:30
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 27-10-2021 21:22:33
 * @ Description: JS tool clock for project IT
 */

 odliczanie();   // starting clock

 function odliczanie() {
     var dzisiaj = new Date();   // geting date
 
     // functions of time from 1970's declared in secods, transform to data type we need to declared on webPage dd/mm/yyyy hh/mm/ss
     var dzien = dzisiaj.getDate();
     var miesiac = dzisiaj.getMonth() + 1; // because counting from 0 to 11
     var rok = dzisiaj.getFullYear();
 
     var godzina = dzisiaj.getHours();
     var minuta = dzisiaj.getMinutes();
     var sekunda = dzisiaj.getSeconds();
 
     if(godzina < 10) godzina = "0" + godzina;
     if(minuta < 10) minuta = "0" + minuta;
     if(sekunda < 10) sekunda = "0" + sekunda;
     if(dzien < 10) dzien = "0" + dzien;
     if(miesiac < 10) miesiac = "0" + miesiac;
 
     document.getElementById("zegar").innerHTML = dzien + "/" + miesiac + "/" + rok + " | " + godzina + ":" + minuta + ":" + sekunda;    // grab id in HTML file and replace this element with clock
     setTimeout("odliczanie()", 1000);   // uppdate interval 1s
 }
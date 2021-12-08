/**
 * @ Author: Maciej Żyrek, Maciej Węcki
 * @ Create Time: 18-11-2021 18:09:06
 * @ Modified by: Maciej Żyrek
 * @ Modified time: 19-11-2021 14:10:14
 * @ Description: AiR 2021/2022, grupa: 7
 */

// getData function

async function getData() {
    // Part1: Get CSV

    // fetch Data
    const response = await fetch("temp.csv");
    // process data
    const rawData = await response.text();
    // publish data
    // document.getElementById("csv").innerHTML = rawData;

    // developer info
    //console.log(rawData);
    //console.log("rawData type: " + typeof rawData);

    // Part 2
    // initialize variables
    let arrayOne = rawData.split("\r\n"); // splits rawData into 3 columns
    arrayOne[0] = "Data,Temp[C],Hum[%]"
    let header = arrayOne[0].split(",");
    let noOfRow = arrayOne.length;
    let noOfCol = 3;
    // console.log(header.length);
    let jsonData = [];
    let i = 0;
    let j = 0;

    // FOR LOOP (ROWS)
    for (let i = 1; i < noOfRow - 1; i++) {
        let obj = {};
        let myNewLine = arrayOne[i].split(",");
        // nested for loop (columns)
        for (let j = 0; j < noOfCol; j++) {
            if(j === 0) myNewLine[j] = odliczanie1(parseInt(myNewLine[j])*1000);  //chaniging data format to 01.01.2021
            obj[header[j]] = myNewLine[j];
        }
        // generate JSON
        jsonData.push(obj);
    }
    // publish data
    // document.getElementById("c").innerHTML = jsonData;

    // developer info
    // console.log(jsonData);
    // console.log(table);
    // constole.log("jsonData type: " + typeof jsonData);



    // PART 3
    // initialize variable
    let children = jsonData;
    let table = document.createElement("table");
    // function to generate table header row
    function addHeaders(table, keys) {
        let row = table.insertRow();
        for (let i = 0; i < keys.length; i++) {
            let cell = row.insertCell();
            cell.appendChild(document.createTextNode(keys[i]));

        }
    }

    // generate table

    for(i =0; i < children.length; i++ ){
        let child = children[i];
        // generate header now
        if(i === 0){
            addHeaders(table,Object.keys(child));
        }

        // generate data rows
        let row = table.insertRow();
        Object.keys(child).forEach(function(k) {
            let cell = row.insertCell();
            cell.appendChild(document.createTextNode(child[k]));
        })

    }

    // publish table
    document.getElementById("csv").appendChild(table);

    // developer info
    console.log("HTML table type: " + typeof table);



};


// call function
getData();
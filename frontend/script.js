function getData() {
  let xhttp = new XMLHttpRequest();
  
  xhttp.open("GET", "http://127.0.0.1:5000/items", true);

  xhttp.onreadystatechange = () => {
    console.log('called');
    console.log('this.status ', xhttp.status);
    console.log('this.readyState ', xhttp.readyState);

    if (xhttp.readyState == 4 && xhttp.status == 200) {
      console.log(this.responseText);

      const jsonData = JSON.parse(xhttp.responseText);
      console.log(jsonData);
      
      const divided = divideData(jsonData);
      console.log(divided);

      drawTables(divided);
    }
  };

  xhttp.send();
}

function simpleTest() {
  const data = JSON.parse('[{"id": 1, "arrival": 11, "shipment": 11, "location": 1337}, {"id": 2, "arrival": 11, "shipment": 15, "location": 1488}]');
  
  const divided = divideData(data);
  drawTables(divided);
}

function divideData(data) {
  let left = [];
  let right = [];

  data.forEach(elem => {
    if (elem["arrival"] === elem["shipment"]) {
      left.push(elem);
    } else {
      right.push(elem)
    }
  });
  
  return [left, right];
}

function drawTables(data) {
  const left = data[0];
  const leftTable = document.getElementById("leftTable");

  const right = data[1];
  const rightTable = document.getElementById("rightTable");

  left.forEach(elem => insertTableRow(elem, leftTable, isShipped = false))
  
  right.forEach(elem => insertTableRow(elem, rightTable, isShipped = true))
}

// VERY BIDLO-CODE :c
function insertTableRow(elem, table, isShipped) {
  const row = table.insertRow(-1);
  
  const cell1 = row.insertCell(0);
  const cell2 = row.insertCell(1);
  const cell3 = row.insertCell(2);
  const cell4 = row.insertCell(3);
  
  cell1.innerHTML = elem["id"];

  const name = elem["name"]
  cell2.innerHTML = name || "-";

  cell3.innerHTML = elem["location"];
  cell4.innerHTML = elem["arrival"];

  if (isShipped) {
    const cell5 = row.insertCell(4);
    cell5.innerHTML = elem["shipment"];
  }
}

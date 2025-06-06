function getTimestamp() {
	let d = new Date();
	const monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
	var datestring = monthNames[(d.getMonth())] + " " + String(d.getDate()).padStart(2, '0') + " " + String(d.getHours()).padStart(2, '0') + ":" + String(d.getMinutes()).padStart(2, '0') + ":" + String(d.getSeconds()).padStart(2, '0') + "." + String(d.getMilliseconds()).padStart(3, '0');
	return datestring;
}

function updateDate() {
	curr_time = getTimestamp();
	document.getElementById("date").innerHTML = "Date: " + curr_time;
}

function fillTable(rawTextTable) {
	const myTable = document.getElementById("procTable");
	var rowCount = myTable.rows.length;
	for (var i = 1; i < rowCount; i++) {
		myTable.deleteRow(1);
	}
	
	//Todo: we 
	const items = rawTextTable.split("\n");
	items.pop(); //how does const work in js? i have no idea honestly
	

	items.forEach( item => {
		const record = item.split(",");

		let row = myTable.insertRow();
		let hostName = row.insertCell(0);
		hostName.innerHTML = record[0];
		let lastPinged = row.insertCell(1);
		lastPinged.innerHTML = record[1];
		let number = row.insertCell(2);
		number.innerHTML = record[2];
		let someData = row.insertCell(3);
		someData.innerHTML = record[3];

		//console.log(record[0] + ", " + record[1]);
	})
}


function getHostList() {
	fetch('http://localhost:1337/get_proc_list', {
		method: 'GET',
		//mode: 'no-cors',
	})
	.then(response => response.text())
	.then(data => {
		//console.log(data);
		fillTable(data);
	});
	updateDate();
}

updateDate();

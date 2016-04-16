function reloadAfter($seconds) {
	setTimeout(function(){
	   window.location.replace("/");
	}, $seconds*1000);
}
function onRangeChange($range, $spanid, $mul, $rotate) {
	var val = document.getElementById($range).value;
	if($rotate) val = document.getElementById($range).max - val;
	document.getElementById($spanid).innerHTML = (val * $mul) + " dB";
}
function onRangeVolChange($value) {
	var val = document.getElementById('vol_range').max - $value;
	document.getElementById('vol1_span').innerHTML = (val * -0.5) + " dB";
	document.getElementById('vol_span').innerHTML = (val * -0.5) + " dB";
	document.getElementById('vol_range').value = $value;
	document.getElementById('vol1_range').value = $value;
	saveSoundSettings();
}
function instantPlay() {
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","instant_play",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("url=" + document.getElementById('instant_url').value + "&port=" + document.getElementById('instant_port').value + "&path=" + document.getElementById('instant_path').value);
	window.location.replace("/");
}

var selindex = 0;
function playStation() {
	select = document.getElementById('stationsSelect');
	selindex = document.getElementById('stationsSelect').options.selectedIndex;
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","play",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("id=" + select.options[select.options.selectedIndex].id);
//	window.location.replace("/");
    window.location.reload(true);
}
function stopStation() {
	var select = document.getElementById('stationsSelect');
	selindex = document.getElementById('stationsSelect').options.selectedIndex;
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","stop",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("id=" + select.options[select.options.selectedIndex].id);
//	window.location.replace("/");
}
function saveSoundSettings() {
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","sound",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("vol=" + document.getElementById('vol_range').value + "&bass=" + document.getElementById('bass_range').value + "&treble=" + document.getElementById('treble_range').value);
//	window.location.replace("/");
}

function saveStation() {
	var file = document.getElementById('add_path').value;
	var url = document.getElementById('add_url').value;
	if (!(file.substring(0, 1) === "/")) file = "/" + file;
	url = url.replace(/^https?:\/\//,'');
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","setStation",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("id=" + document.getElementById('add_slot').value + "&url=" + url + "&name=" + document.getElementById('add_name').value + "&file=" + file + "&port=" + document.getElementById('add_port').value);
	window.location.replace("/");
}
function editStation(id) {
	document.getElementById('add_slot').value = id;
	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var arr = JSON.parse(xmlhttp.responseText);
			document.getElementById('add_url').value = arr["URL"];
			document.getElementById('add_name').value = arr["Name"];
			document.getElementById('add_path').value = arr["File"];
			document.getElementById('add_port').value = arr["Port"];
			document.getElementById('editStationDiv').style.display = "block";
			setMainHeight("tab-content2");
		}
	}
	xmlhttp.open("POST","getStation",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("id=" + id);
}
function loadStations(page) {
	var new_tbody = document.createElement('tbody');
	var id = 16 * (page-1);
	for(id; id < 16*page; id++) {
		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function() {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				var arr = JSON.parse(xmlhttp.responseText);
				var tr = document.createElement('TR');
				var td = document.createElement('TD');
				td.appendChild(document.createTextNode(id + 1));
				td.style.width = "10%";
				tr.appendChild(td);
				for(var key in arr){
					var td = document.createElement('TD');
					if(arr[key].length > 64) arr[key] = "Error";
					td.appendChild(document.createTextNode(arr[key]));
					tr.appendChild(td);
				}
				var td = document.createElement('TD');
				td.innerHTML = "<a href=\"#\" onClick=\"editStation("+id+")\">Edit</a>";
				tr.appendChild(td);
				new_tbody.appendChild(tr);
			}
		}
		xmlhttp.open("POST","getStation",false);
		xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlhttp.send("id=" + id);
	}
//	console.log(new_tbody);
	var old_tbody = document.getElementById("stationsTable").getElementsByTagName('tbody')[0];
	old_tbody.parentNode.replaceChild(new_tbody, old_tbody);
}
	
function getSelIndex() {
		xmlselindex = new XMLHttpRequest();
		xmlselindex.onreadystatechange = function() {
			if (xmlselindex.readyState == 4 && xmlselindex.status == 200) {
//				console.log("JSON: " + xmlselindex.responseText);
				var arr = JSON.parse(xmlselindex.responseText);
				if(arr["Index"].length > 0) {
					document.getElementById("stationsSelect").options.selectedIndex = arr["Index"];
					document.getElementById("stationsSelect").disabled = false;
//					console.log("selIndex received " + arr["Index"]);
				} 
			}
		}
		xmlselindex.open("POST","getSelIndex",false);
		xmlselindex.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlselindex.send();	
}	
function loadStationsList(max) {
	var foundNull = false;
	for(var id=0; id<max; id++) {
		if (foundNull) break;
		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function() {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				var arr = JSON.parse(xmlhttp.responseText);
				if(arr["Name"].length > 0) {
					var opt = document.createElement('option');
					opt.appendChild(document.createTextNode(arr["Name"]));
					opt.id = id;
					document.getElementById("stationsSelect").appendChild(opt);
				} else foundNull = true;
			}
		}
		document.getElementById("stationsSelect").disabled = true;
		xmlhttp.open("POST","getStation",false);
		xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlhttp.send("id=" + id);
	}
	getSelIndex();


}
function setMainHeight(name) {
	var minh = window.innerHeight;
	var h = document.getElementById(name).offsetHeight + 200;
	if(h<minh) h = minh;
	document.getElementById("MAIN").style.height = h;
}
function getMetadata() {
	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var metadata = xmlhttp.responseText;
			var start = metadata.indexOf("StreamTitle=");
			if(start != -1) {
				start = start + 13;
				var stop = metadata.indexOf(";", start) - 1;
				document.getElementById("meta_title").innerHTML = metadata.substring(start, stop);
			}
		}
	}
	xmlhttp.open("POST","getMetadata",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send();	
}
document.addEventListener("DOMContentLoaded", function() {
	document.getElementById("tab1").addEventListener("click", function() {
			setMainHeight("tab-content1");
		});
	document.getElementById("tab2").addEventListener("click", function() {
			loadStations(1);
			setMainHeight("tab-content2");
		});
	document.getElementById("tab3").addEventListener("click", function() {
			setMainHeight("tab-content3");
			onRangeChange('treble_range', 'treble_span', 1.5, false);
			onRangeChange('bass_range', 'bass_span', 1, false);
			onRangeChange('vol_range', 'vol_span', -0.5, true);
		});

//	setMainHeight("tab-content1");
//	onRangeChange('treble_range', 'treble_span', 1.5, false);
//	onRangeChange('bass_range', 'bass_span', 1, false);
//	onRangeChange('vol_range', 'vol_span', -0.5, true);
//	loadStations(1);
	loadStationsList(192);
	onRangeChange('vol1_range', 'vol1_span', -0.5, true);
	setMainHeight("tab-content1");
	getMetadata();
	setInterval(getMetadata, 5000);
});

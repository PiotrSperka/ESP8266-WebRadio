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
function instantPlay() {
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","instant_play",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("url=" + document.getElementById('instant_url').value + "&port=" + document.getElementById('instant_port').value + "&path=" + document.getElementById('instant_path').value);
	window.location.replace("/");
}
function playStation() {
	var select = document.getElementById('stationsSelect');
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","play",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("id=" + select.options[select.options.selectedIndex].id);
	window.location.replace("/");
}
function saveSoundSettings() {
	xmlhttp = new XMLHttpRequest();
	xmlhttp.open("POST","sound",false);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("vol=" + document.getElementById('vol_range').value + "&bass=" + document.getElementById('bass_range').value + "&treble=" + document.getElementById('treble_range').value);
	window.location.replace("/");
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
	console.log(new_tbody);
	var old_tbody = document.getElementById("stationsTable").getElementsByTagName('tbody')[0];
	old_tbody.parentNode.replaceChild(new_tbody, old_tbody);
}
function loadStationsList(max) {
	for(var id=0; id<max; id++) {
		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function() {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				var arr = JSON.parse(xmlhttp.responseText);
				var opt = document.createElement('option');
				opt.appendChild(document.createTextNode(arr["Name"]));
				opt.id = id;
				document.getElementById("stationsSelect").appendChild(opt);
			}
		}
		xmlhttp.open("POST","getStation",false);
		xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlhttp.send("id=" + id);
	}
}
function setMainHeight(name) {
	var minh = window.innerHeight;
	var h = document.getElementById(name).offsetHeight + 200;
	if(h<minh) h = minh;
	document.getElementById("MAIN").style.height = h;
}
document.addEventListener("DOMContentLoaded", function(event) {
	document.getElementById("tab1").addEventListener("click", function() {
			setMainHeight("tab-content1");
		});
	document.getElementById("tab2").addEventListener("click", function() {
			setMainHeight("tab-content2");
		});
	document.getElementById("tab3").addEventListener("click", function() {
			setMainHeight("tab-content3");
		});
	setMainHeight("tab-content1");
	onRangeChange('treble_range', 'treble_span', 1.5, false);
	onRangeChange('bass_range', 'bass_span', 1, false);
	onRangeChange('vol_range', 'vol_span', -0.5, true);
	loadStations(1);
	loadStationsList(8);
});

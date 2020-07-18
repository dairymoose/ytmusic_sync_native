
var currentSongTitle = '';
var reportedLength = 0;
var currentData = '';

function submitUpload(msgPort, songTitle, length, data) {
	var tabUrl;
	var tabId;
	chrome.tabs.query({}, function(tabs) {
		for (var i = 0; i < tabs.length; i++) {
			tabUrl = tabs[i].url;
			if (tabUrl.startsWith('https://music.youtube.com/')) {
				tabId = tabs[i].id;
				console.log('Begin file upload...');
				console.log('Received bytes: ' + data.length + ', reported bytes: ' + length);
				if (data.length != length) {
					console.log('Length mismatch!  Aborting upload!');
				} else {
					var n = Date.now();
					
					var encodedSongTitle = window.btoa(songTitle);
					
					var codeText = 'console.log("uploading file...");' +
					'var encodedSongTitle="' + encodedSongTitle + '";' +
					'var encoded="' + data + '";' +
					'var decoded=window.atob(encoded);' +
					'var songTitle=window.atob(encodedSongTitle);' +
					'var rawLength = decoded.length;' +
					'var rawArray = new Uint8Array(new ArrayBuffer(decoded.length));' +
					'for(i = 0; i < rawLength; i++) {' + 
					'rawArray[i] = decoded.charCodeAt(i);' + 
					'}' + 
					'var f = new File([rawArray], songTitle, {lastModified: ' + n + ', type: "audio/mpeg"});' +
					'var uc = document.querySelector("ytmusic-app").uploadController_;' +
					'var input=uc.fileSelectorOverlay.overlay_;' +
					'var list = new DataTransfer();list.items.add(f);input.files = list.files;' +
					'var a={charCode: 0,currentTarget: input,defaultPrevented: false,event_: null,key: "",keyCode: 0,platformModifierKey: undefined,pointerId: 0,pointerType: "",propagationStopped_: false,relatedTarget: undefined,screenX: 0,screenY: 0,target: input,type: "change"};' +
					'console.log(f);' + 
					'uc.fileSelectorOverlay.onFilesSelected_(a);';
		
					var codeText2= 'var myJavaScript = \'' + codeText + '\';' +
						'var scriptTag = document.createElement("script");' +
						'scriptTag.innerHTML = myJavaScript;'+
						'document.head.appendChild(scriptTag);';
		
					chrome.tabs.executeScript(tabId, {code: codeText2, allFrames : true});
					
					//chrome.runtime.sendNativeMessage('com.ytmusic.sync.native',{ text: "ready" });
					
				}
				return;
			}
		}
	});
}

chrome.runtime.onInstalled.addListener(function() {
	var port = chrome.runtime.connectNative('com.ytmusic.sync.native');
	console.log('Started');
	port.onMessage.addListener(function(msg, msgPort) {
	console.log("Received content:");
	if (msg.x != null) {
		console.log("msg len = " + msg.x.length);
	}
	else if (msg.m != null) {
		if (msg.m.charAt(0) == '1') {
			var r = msg.m.split('/');
			if (r.length == 2) {
				console.log("Begin receive file: " + r[1]);
				currentSongTitle = r[1];
			}
		}
		else if (msg.m.charAt(0) == '2') {
			var r = msg.m.split('/');
			if (r.length == 2) {
				console.log("Finished receive file: len = " + r[1]);
				reportedLength = r[1];

				submitUpload(msgPort, currentSongTitle, reportedLength, currentData);
				
				currentSongTitle = '';
				reportedLength = 0;
				currentData = '';
				
				msgPort.postMessage({ text: "ready" });
			}
		}
	}
	else if (msg.f != null) {
		console.log("filepart len = " + msg.f.length);
		currentData += msg.f;
	}
	});
	port.onDisconnect.addListener(function() {
	  console.log("Disconnected");
	});
	//port.postMessage({ text: "hello world" });
	});
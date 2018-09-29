/**
 * Personal device section
 */
cr.define("options", function() {
 
  function PersonalDevice(data){
	  
  }
  
  cr.addSingletonGetter(PersonalDevice);
  
  PersonalDevice.prototype = {
  
	initialize: function(){
	  // notify browser to return device data
	  chrome.send('requestLoginDeviceInfo');
	},
	
	/**
	*	get data from browser to init personal-device-section
	*/
	afterInit: function (data) {
	  data = data || [];
	  this.$wrap = $("device-data-wrap");
	  this.data = data;
	  this.buildDOM();
	},
	
	buildDOM:function(){
      var _ = this;
	  let data = this.data;
	  let ouNode = document.createElement('ul');
	  ouNode.id = 'ou-li-id'
	  data.forEach(function(device){
		let liNode = document.createElement('li');
		//liNode.innerHTML = "<span>" + device.model + "</span>"
		let imgNode = document.createElement('img');
		imgNode.id = device.id;
		if(device.platform == 2 && device.operatingSystem.indexOf('iOS') !== -1){
			imgNode.className = "iphone-img";
		}else if(device.platform == 1 && device.operatingSystem.indexOf('OS') !== -1){
			imgNode.className = "macbook-img";
		}else if(device.platform == 2 && device.operatingSystem.indexOf('Android') !== -1){
			imgNode.className = "android-img"; 
		}else if(device.platform == 1 && device.operatingSystem.indexOf('Android') !== -1 && device.operatingSystem.indexOf('Pad') !== -1){
			imgNode.className = 'pad-android-img';
		}else if(device.platform == 1 && device.operatingSystem.indexOf('Windows') !== -1){
			imgNode.className = "windows-pc-img"; 
		}else {
			imgNode.className = "windows-pc-img"; 
		}
		imgNode.addEventListener("click",function(){
			_.showPrompt(liNode,device.id);
		});
		
		liNode.appendChild(imgNode);
		ouNode.appendChild(liNode);
	  })
	  this.$wrap.appendChild(ouNode);
	},
	
	showPrompt:function(liNode,dId){
	  var _ = this;
	  if($("tip-id") === null){
		  let data = this.data;
		  let dObj = {};
		  for(let i = 0; i < data.length; i++){
			let o = data[i];
			if(dId == o.id){
			  dObj = o;
			  break;
			}
		  }
		  console.log("test click222");
		  let tipNode = document.createElement('div');
		  tipNode.className = 'tip';
		  tipNode.id = 'tip-id';
		  
		  let innerNode = document.createElement('div');
		  innerNode.className = 'inner';
		  tipNode.appendChild(innerNode);
		  
		  let dNameNode = document.createElement('div');
		  dNameNode.className = 'device-name';
		  dNameNode.innerHTML = '<span>' + dObj.model +'</span>'
		  tipNode.appendChild(dNameNode);
		  let dInfoNode = document.createElement('div');
		  dInfoNode.className = 'device-information';
		  for(let j = 0; j < 4; j++){
			var pNode = document.createElement('p');
			if(j == 0){
				pNode.innerHTML = "设备型号 ： " + dObj.model;
			}
			if(j == 1){
				pNode.innerHTML = "操作系统 ： " + dObj.operatingSystem;
			}
			if(j == 2){
				pNode.innerHTML = "客户端版本 ： " + dObj.clientVersion;
			}
			if(j == 3){
				pNode.innerHTML = "设备ID ： " + dObj.id;
			}
			dInfoNode.appendChild(pNode);
		  }
		  tipNode.appendChild(dInfoNode);
		  //tipNode.innerHTML = '设备信息在这里';
		  
		  let dButtonNode = document.createElement('div');
		  dButtonNode.className = 'device-delete-buttion';
		  let bDelNode = document.createElement('div');
		  bDelNode.className = 'delte-device-node';
		  bDelNode.innerHTML = "<button class='default-button' i18n-content='ok'>" + "删除该设备" + "</button>";  
		  bDelNode.addEventListener("click",function(){
			_.deleteDeviceDiv(dObj.id);
		  });
		  
		  dButtonNode.appendChild(bDelNode);
		  
		  tipNode.appendChild(dButtonNode);
		  
		  liNode.appendChild(tipNode);
	  }else{
	    $("tip-id").parentNode.removeChild($("tip-id"));
	  }
	},
	
	deleteDeviceDiv: function(dId){
	  var _ = this;
	  $("tip-id").style.display = "none";
	  $("device-data-bg-id").style.display = "block";
	  $("confirm-buttion-id").addEventListener("click",function(){
			_.deleteDeviceOperation(dId);
	  });
	  
	  $("cancel-buttion-id").addEventListener("click",function(){
			_.cancelDeleteOperation();
	  });
	},
	
	deleteDeviceOperation: function(dId){
	   // notify browser to delete device data
	   chrome.send('deleteLoginDevice',[dId]);
	   $("device-data-bg-id").style.display = "none";
	},
	cancelDeleteOperation: function(){
	  $("device-data-bg-id").style.display = "none";
	},

	requestLoginDeviceInfoCallback_ : function(data) {
	  if (!data) {
		var arr = [];
	  } else {
		let obj = JSON.parse(data);
        var arr = obj.data || [];
	  }
	  console.log(arr);
	  if($("ou-li-id") != null){
	    $("ou-li-id").parentNode.removeChild($("ou-li-id"));
	  }
      this.afterInit(arr);
    },
	deleteLoginDeviceCallback_:function(data){
	  let obj = JSON.parse(data);
	  chrome.send('requestLoginDeviceInfo');
	}
  };
  
  // Forward public APIs to private implementations on the singleton instance.
  cr.makePublic(PersonalDevice, [
    "requestLoginDeviceInfoCallback",
	"deleteLoginDeviceCallback"
  ]);
  
  return {
	PersonalDevice: PersonalDevice
  };
});

/**
 * Personal data section
 */
cr.define("options", function() {
  /** @const */ var Page = cr.ui.pageManager.Page;
  /** @const */ var PageManager = cr.ui.pageManager.PageManager;

  /**
   * send data
   * @param {*} url
   * @param {*} options
   */
  function Send(url, options) {
    var method = options.method;
    var data = options.data;
    var success = options.success;
    var error = options.error;
    var progress = options.progress;
    var contentType = options.contentType;

    var upload = new XMLHttpRequest();

    upload.open(method, url, true);
    upload.setRequestHeader("Content-Type", contentType);
    upload.addEventListener("load", function() {
      success(this.responseText);
    });
    upload.addEventListener("error", function() {
      error(this.responseText);
    });

    upload.send(data);
  }

  /**
   * Input field0
   * @param {obj} {name, title, value}
   * @param {obj} data(optional)
   */
  function Field(label, data) {
    this.label = label;
    this.data = data;

    this.name = "personal-data-" + label.name;
    this.title = label.title;
    this.value = label.value;
    this.isChanged = data.isChanged;

    this.$outWrap = $("personal-data-wrap");
    this.$wrap = document.createElement("div");
    this.$wrap.id = this.name;
    this.$wrap.className = "personal-data-label";

    this.titleID = this.name + "-title";
    this.showID = this.name + "-show";
    this.fieldID = this.name + "-field";
    this.editID = this.name + "-edit";
    this.confirmID = this.name + "-confirm";
    this.cancelID = this.name + "-cancel";

    this.initialize();
  }

  Field.prototype = {
    get DOM() {
      return this.wrap;
    },

    /**
     * DOM and Events
     */
    initialize: function() {
      this.buildDOM();

      if (this.isChanged) {
        this.onEvents();
        this.handleField(this.label.value);
      }
    },

    /**
     * build DOM
     */
    buildDOM: function() {
      var name = this.name;

      var title =
        '<span id="' +
        name +
        '-title" class="personal-data-title">' +
        this.title +
        "</span>";
      var show =
        '<span id="' +
        name +
        '-show" class="personal-data-show">' +
        this.value +
        "</span>";
      var field =
        '<input id="' +
        name +
        '-field" class="personal-data-field" type="text">';
      var edit =
        '<span id="' + name + '-edit" class="personal-data-edit"></span>';
      var editConfirm =
        '<span id="' + name + '-confirm" class="personal-data-confirm"></span>';
      var editCancel =
        '<span id="' + name + '-cancel" class="personal-data-cancel"></span>';

      if (this.isChanged) {
        this.$wrap.innerHTML =
          title + show + field + edit + editConfirm + editCancel;
      } else {
        this.$wrap.innerHTML = title + show 
      }

      this.$outWrap.appendChild(this.$wrap);
      //console.log("build DOM %s::: success", this.label.name);
    },

    /**
     * bind events for different Elements
     */
    onEvents: function() {
      var _ = this;

      if (!this.isChanged) {
        return
      }

      // edit user data
      $(this.editID).addEventListener("click", function() {
          //console.log("editing");
          _.onEditing();
        });

      // cancel editing
      $(this.cancelID).addEventListener("click", function() {
        //console.log("cancel");
        _.handleCancel();
      });

      // confirm editing
      $(this.confirmID).addEventListener("click", function() {
        //console.log("confirm");
        _.handleConfirm();
      });
    },

    /**
     * 1. sync value to SHOW and input
     * 2. notify the browser
     */
    handleConfirm: function() {
      var value = this.handleField();
      //console.log("changed Value", value);

      Send({
        url: "",
        method: "POST",
        data: ""
      });

      // chrome.send('change', [value])
      this.value = value;
      $(this.showID).innerHTML = value;
      this.offEditing();
    },

    /**
     * cacel editing
     */
    handleCancel: function() {
      this.handleField(this.value);

      this.offEditing();
    },

    handleField: function(value) {
      if (value) {
        $(this.fieldID).value = value;
      }
      return $(this.fieldID).value;
    },

    onEditing: function() {
      $(this.showID).style.display = "none";
      $(this.editID).style.display = "none";
      $(this.confirmID).style.display = "inline-block";
      $(this.cancelID).style.display = "inline-block";
      $(this.fieldID).style.display = "inline-block";
    },

    offEditing: function() {
      $(this.showID).style.display = "inline-block";
      $(this.editID).style.display = "inline-block";
      $(this.confirmID).style.display = "none";
      $(this.cancelID).style.display = "none";
      $(this.fieldID).style.display = "none";
    }
  };



  function PersonalData(data) {

  }

  cr.addSingletonGetter(PersonalData);
  

  PersonalData.prototype = {

    __proto__: Page.prototype,

    initialize: function() {
      // notify browser to return data
       chrome.send('requestUserInfo')
    },

    /**
     * get Data from browser to init personal-data-section
     */
    afterInit: function (data) {
      data = data ? JSON.parse(data) : "";
      this.isLogin = data ? true : false;
      this.isChanged = data.isChanged || false;
      this.$wrap = $("personal-data-wrap");
      this.$loginBtn = $("personal-data-login");
      this.$changePswBtn = $('personal-data-psw');
      this.data = data;
      
      var _ = this;

      this.$loginBtn.addEventListener("click", function() {
        _.toggleLogin();
      });

      this.$changePswBtn.addEventListener("click", function () {
          //console.log('change psw btn click');
          _.popLayerChangePsw();
      });

      if (this.isLogin) {
        this.buildDOM();
        this.$loginBtn.innerHTML = "退出登录";
        this.$changePswBtn.innerHTML = "修改密码";
      } else {
        this.notLogin();
        return
      }

    },

    /**
     * build form DOM
     */
    buildDOM: function() {
      var data = this.data;

      var fields = {
        name: {
          name: "name",
          title: "姓名",
          value: data.name
        },
        email: {
          name: "email",
          title: "邮箱",
          value: data.email
        },
        mobile: {
          name: "mobile",
          title: "手机号码",
          value: data.mobile
        },
        avatar: {
          url: data.avatarPath,
		  headName: data.headName,
		  headColor: data.headColor
        }
      };

      this.$wrap.innerHTML = "";

      var name = new Field(fields.name, data);
      var email = new Field(fields.email, data);
      var mobile = new Field(fields.mobile, data);

      this.buildAvatar(fields.avatar);
    },

    /**
     * build avatar DOM
     */
    buildAvatar: function(avatarData) {
      var data = this.data;
      var avatarNode = document.createElement("div");
      avatarNode.id = "personal-data-avatar";
      avatarNode.className = "personal-data-label";

      var titleNode = document.createElement("span");
      titleNode.textContent = "头像";
      titleNode.className = "personal-data-title";

      var imgwrapNode = document.createElement("div");
      imgwrapNode.id = "personal-data-avatar-preview-wrap";
	  
	  function getColor(number) {
        let color = number;
        const blue = parseInt(color % 0x100, 10);
        color = color >>> 8;
        const green = parseInt(color % 0x100, 10);
        color = color >>> 8;
        const red = parseInt(color % 0x100, 10);
        const alpha = (parseInt(color >>> 8, 10) / 0xFF).toFixed(1);
        return `rgba(${red}, ${green}, ${blue}, ${alpha})`;
      }
		
	  var imgNode = '';
	  if (avatarData.url) {
		imgNode = document.createElement("img");
		this.imgNode = imgNode;
		imgNode.id = "personal-data-avatar-preview";
		imgNode.alt = "avatar";
		imgNode.src =
		avatarData.url ||
		"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABoAAAAgCAMAAAA7dZg3AAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAK7OHOkAAAB+UExURUxpcczMzM3NzczMzOHh4czMzMzMzM3NzdHR0czMzMzMzMzMzM7OzszMzM3Nzc3NzdXV1c3NzdbW1s3NzdTU1MzMzM3NzczMzP///83NzczMzMzMzMzMzMzMzM3NzczMzM3Nzc/Pz8zMzM3Nzc7OztDQ0MzMzM3NzczMzMzMzE2VTu8AAAApdFJOUwDadeYE98eFGP65rDzgv30JiQ5oEfNM7wKUeaai+zNBSCOy0FEsl1hHIaHB9gAAARhJREFUeNptkVmOgzAUBM1ijNlDQkiA7NvU/S84GoLBzqS/Wiqp3E8WJufBk7q9lZX4SFwwJdrWDtl4EAylrxKg3dqoR5fh2JQGeVzIA8pZkAXcdjNaEYRiGQR3YRKxt/UF17kfeNloTWtqDZmNfsD4YzjaaIs2tQLnFJ987h7KRimptTCxUbSgULN2hY2pu8Mnauee4NUL2QTWWyfoF5SgrVtW8DA9g86yhwHPZbon7Chk/G6XA76DYklUj+VGsxFOfFDTq2vhpgpYvc155ei6qIHztK+JutjMUxIwH70HkCocVQnoQp3m+1WhIfnT9pBchJPLFfpR/nTBpM3EgBf+R6HHIHJK8SUduZDcv6E7UqR5/A3FefoL4ZYajPpmOsYAAAAASUVORK5CYII=";
	  } else {
		imgwrapNode.innerHTML = avatarData.headName;
		imgwrapNode.style.background = getColor(avatarData.headColor);
		imgwrapNode.style.color = '#fff';
	  }

      
      var uploadBtnNode = document.createElement("span");
      uploadBtnNode.id = "upload-avatar-trigger";
      uploadBtnNode.textContent = "修改图像";
      
      // if user can change data
      if (this.isChanged) {
        uploadBtnNode.onclick = function() {
          //console.log('trigger upload')
          PageManager.showPageByName('upload');
        }
      } else {
        uploadBtnNode.style.display = 'none';
      }
		
	  if (imgNode) {
		imgwrapNode.appendChild(imgNode);
	  }
      
      avatarNode.appendChild(titleNode);
      avatarNode.appendChild(imgwrapNode);
      avatarNode.appendChild(uploadBtnNode);

      this.$wrap.className = 'personal-data-login';
      this.$wrap.appendChild(avatarNode);
    },

    /**
     * update avatar src
     */
    updateAvatar: function(url) {
      this.imgNode.src = url;
    },

    /**
     * Notify the browser to Login-page
     */
    toggleLogin: function() {
      //console.log("toggle login", this.isLogin);
      if (this.isLogin) {
        this.isLogin = false;
        this.notLogin();
        chrome.send("logout");
      } else {
        this.login();
      }
    },

    login: function() {
      window.top.location = "redcore://newtab"
    },

    popLayerChangePsw: function () {
        chrome.send('checkIfCanModifyPassword');
    },

    checkIfCanModifyPasswordCallback_: function (d) {
        //console.log(d);
        if (d === 'false') {
            $('rdc_reset_frm').style.display = 'none';
            $('rdc_reset_status').style.display = 'block';
        }
        PageManager.showPageByName('resetpsw');
    },


    /**
     * reset personal-data
     */
    notLogin: function() {
      this.$wrap.className = 'personal-data-logout';
      this.$wrap.innerHTML = "当前状态未登录";
      this.$loginBtn.innerHTML = "登录";
      this.$changePswBtn.style.display = "none";
    },

    requestUserInfoCallback_: function(data) {
      this.afterInit(data)
    }
  };


  // Forward public APIs to private implementations on the singleton instance.
  cr.makePublic(PersonalData, [
      "checkIfCanModifyPasswordCallback",
    "requestUserInfoCallback" //include: userid companyid name email mobile avatar
  ]);

  return {
    PersonalData: PersonalData
  };
});


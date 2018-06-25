<<<<<<< HEAD
/**
 * if login, directly show Nav page
 */
function switchPage(isLogin) {
  if (isLogin) {
    $('#login').css('display', 'none');
    $('#nav').show();
  } else {
    $('#login').css('display', 'flex');
    $('#nav').hide();
  }
}

/*
* Login module Constructor
*/
function Login() {
    // deviceId = '1111';
    var serverAddress = '';
    var _this = this;
    var step = 1;
    // Input-Validator Constructor
    function Validator(id) {
      this.$input = $(id);
      this.state = false;
  
      this.$input.focus(function() {
        $(this).removeClass('error')
        _this.errorTip('')
      })
    }
  
    Validator.prototype.verify = function() {
      var _ = this;

      this.$input.change(function() {
        var pattern = $(this).attr('pattern')
        var tip = $(this).attr('tip')
        var value = $(this).val()
        if (!RegExp(pattern).test(value)) {
          _.state = false
          $(this).addClass('error')
        } else {
          _.state = true
        }
      })
    }

    Validator.prototype.val = function(data) {
      if (data) {
        this.state = true
        this.$input.val(data)
      }
      return this.$input.val()
    }
  
    this.domain = new Validator('#domain')
    this.userid = new Validator('#userid')
    this.password = new Validator('#password')
    this.address = new Validator('#address')

    // login
    var loginServer = function () {


        if (!_this.password.val()) {
            _this.errorTip('请输入登录密码');
            return;
        }
        if (!_this.userid.val()) {
            _this.errorTip('请输入用户名');
            return;
        }
        var domain = _this.domain.val()
        if (domain) {
            if (!/\/\//.test(domain)) {
                domain = 'http://' + domain
            }
        } else {
			// 正式地址
            domain = 'https://api.redcore.cn'
			
			// 测试地址
			//domain = 'http://api.enterplorer.net'
        }
        HOMEPROXY.login(domain, _this.userid.val(), _this.password.val());
        _this.loading();
    }

    var domainVer = function () {
        // if (!_this.domain.val()) {
        //     _this.errorTip('服务器地址输入不正确');
        //     return;
        // }
        step = 2;
        $('#company_domain').css('display', 'none');
        $('#company_userMessage').css('display', 'block');
        _this.errorTip('');
    }
    // 填入企业域名点击下一步
    $('#next-btn').click(function() {
        domainVer();
    });

    // 返回上一步添加企业域名
    $('#domain-back').click(function() {
        step = 1;
        _this.errorTip('');
        $('#company_domain').show();
        $('#company_userMessage').hide();
    })

    // 通过隐藏按钮来 设置 页面js和content_script的通信
    var getProxyEl = function () {
        return top.document.getElementById('btn_page_to_content_script');
    };
    // 在dom中写入数据
    window.__page_to_content_script__ = function (url, deviceId) {
        var proxy = getProxyEl();
        proxy.setAttribute('data-type', '__page_to_content_script__');
        proxy.setAttribute('data-url', url);
        proxy.setAttribute('data-deviceId', deviceId);
        proxy.click();
    };


    // 接收来自 content_script的 信息
    document.getElementById('btn_content_script_to_page').addEventListener('click', function (e) {
        var type = this.getAttribute('data-type');
        switch (type) {
            case '__content_script_to_page___':
                var flag = this.getAttribute('data-flag');
                // 扫码成功
                if (flag === 'scanned') {

                    $('#code_pack').hide();
                    $('#code_pack_qrcodeInvalid').hide();
                    $('#code_pack_qrcodeSuccess').show();
                }
                // 过期
                if (flag === 'timeout' || flag === 'cancelled') {
                    $('#code_pack').hide();
                    $('#code_pack_qrcodeSuccess').hide();
                    $('#code_pack_qrcodeInvalid').show();
                }
                break;
        }
    }, false);

    // 生成二维码
    var qrcode = new QRCode(document.getElementById("qrcode_img"), {
        width : 150,
        height : 150
    });
    // // 点击二维码
    $('#QRcode').click(function() {
        // if (!_this.domain.val()) {
        //     _this.errorTip('设置服务器地址');
        //     return;
        // }
        serverAddress = _this.domain.val() || 'api.redcore.cn';
        // 建立长连接
        window.__page_to_content_script__('http://' + serverAddress, deviceId);
        $('#code_pack_qrcodeInvalid').hide();
        $('#code_pack_qrcodeSuccess').hide();
        $('#code_pack').css('display', 'flex');
        _this.errorTip('');
        // 获取二维码
        $.ajax({
            method: "GET",
            url: "http://" + serverAddress + '/client/v3/push/qrlogin/qrcode?deviceId=' + deviceId,
            success: function(res) {
                var resData = JSON.parse(res);
                if (resData.qrcode) {
                    qrcode.makeCode(resData.qrcode);
                    $('#qrcode_img').attr("title"," ");
                } else {
                    _this.errorTip('获取二维码失败 ');
                }
            },
            error: function(err) {
                _this.errorTip('获取二维码失败 ');
                for (var i = 0; i < $("#qrcode_img>img").length; i ++) {
                    $("#qrcode_img>img")[i].style.display = 'none'
                }
            }
        });
        $('#company_QRcode').css('display', 'block');
        $('#company_userMessage').css('display', 'none');
        $('#company_domain').css('display', 'none');
    });

    // 刷新重新获取二维码
    $('#refresh_qrcode').click(function() {
        $.ajax({
            method: "GET",
            url: "http://" + serverAddress + '/client/v3/push/qrlogin/qrcode?deviceId=' + deviceId,
            success: function(res) {
				var resData = JSON.parse(res);
                if (resData.qrcode) {
                    qrcode.makeCode(resData.qrcode);
                    $('#qrcode_img').attr("title"," ");
					$('#code_pack_qrcodeInvalid').hide();
                    $('#code_pack_qrcodeSuccess').hide();
                    $('#code_pack').css('display', 'flex');
                } else {
                    _this.errorTip('获取二维码失败 ');
                }
            },
            error: function(err) {
                _this.errorTip('获取二维码失败 ');
            }
        });
    });

    $('#back_login').click(function() {
        _this.errorTip('');
        $('#company_QRcode').css('display', 'none');
        $('#company_domain').css('display', 'none');
        $('#company_userMessage').css('display', 'block');
    });


    $('#login-btn').click(function() {
      loginServer();
    });

    $('#code_pack').mousemove(function(e){
        $('#code_tip').show();
    });
    $('#code_pack').mouseleave(function(e){
        $('#code_tip').hide();
    });

    window.addEventListener('keydown', function (e) {
      if ($('#login').css('display') === "flex" && e.which == 13) {
          if (step == 1) {
              domainVer();
          } else {
              _this.errorTip('');
              loginServer();
          }
      }
    })

    // show manager-server-address setting page
    var showServerSetting = function() {
      $('#form').hide();
      $('#server').show();
    }
    var hideServerSetting = function() {
      $('#form').show();
      $('#server').hide();
    }

    $('#auto-login-checkbox').change(function() {
      HOMEPROXY.setAutoLogin(this.checked)
    })

    $('#confirm-server-address').click(function() {
       HOMEPROXY.setManagerAddress(_this.address.val())
       hideServerSetting()
    })

    // show or hide server-address setting page
    $('#server-setting').click(function() {
        $('#company_domain').show();
        $('#company_userMessage').hide();
        _this.errorTip('');
    })
    // $('#server-back').click(function() {
    //  hideServerSetting()
    // })

    // 打开提示忘记密码弹框
    $('#openTip').click(function () {
        $('#forgetPassword').show();
    });
    $('#closeTip').click(function () {
        $('#forgetPassword').hide();
    });

}


// ++++++++++++++++++++
// LOGIN = new Login();
/**
 * handle loading status
 */
Login.prototype.removeLoading = function() {
  $('#login-btn').text('登 录');
  $('#login-btn').removeClass('login-button');
}
Login.prototype.loading = function() {
  $('#login-btn').text('登录中');
  $('#login-btn').addClass('login-button');
}

/**
 * set default value
 * @param {*} data 
 */
Login.prototype.setLoginInfo = function(data) {
  this.domain.val(data.domain)
  this.userid.val(data.userid)
}
Login.prototype.setAutoLoginStatus = function(data) {
  document.querySelector('#auto-login-checkbox').checked = data
}
Login.prototype.setManagerAddress = function(data) {
  $('#address').val(data);
}

/**
 * set error tip for user input
 * @param {string} error info
 */
Login.prototype.errorTip = function(data) {
    if (data) {
        $('#error-row').css('background', '#fee6e6');
        $('#error-tip').html("<img src=\"./imgs/error_icon.png\" alt=\"\">" + data);
    } else {
        $('#error-row').css('background', 'none');
        $('#error-tip').html('');
    }
}
||||||| parent of ffef05f456bc... Add untracked files after rebase
=======
/**
 * if login, directly show Nav page
 */
function switchPage(isLogin) {
  if (isLogin) {
    $('#login').css('display', 'none');
    $('#nav').show();
  } else {
    $('#login').css('display', 'flex');
    $('#nav').hide();
  }
}

/*
* Login module Constructor
*/
function Login() {
    // deviceId = '1111';
    var serverAddress = '';
    var _this = this;
    var step = 1;
    // Input-Validator Constructor
    function Validator(id) {
      this.$input = $(id);
      this.state = false;
  
      this.$input.focus(function() {
        $(this).removeClass('error')
        _this.errorTip('')
      })
    }
  
    Validator.prototype.verify = function() {
      var _ = this;

      this.$input.change(function() {
        var pattern = $(this).attr('pattern')
        var tip = $(this).attr('tip')
        var value = $(this).val()
        if (!RegExp(pattern).test(value)) {
          _.state = false
          $(this).addClass('error')
        } else {
          _.state = true
        }
      })
    }

    Validator.prototype.val = function(data) {
      if (data) {
        this.state = true
        this.$input.val(data)
      }
      return this.$input.val()
    }
  
    this.domain = new Validator('#domain')
    this.userid = new Validator('#userid')
    this.password = new Validator('#password')
    this.address = new Validator('#address')

    // login
    var loginServer = function () {

        if (!_this.userid.val()) {
            _this.errorTip('请输入个人账号');
            return;
        }
        if (!_this.password.val()) {
            _this.errorTip('请输入密码');
            return;
        }
        var domain = _this.domain.val()
        if (domain) {
            if (!/\/\//.test(domain)) {
                domain = 'http://' + domain
            }
        } else {
			// 正式地址
            domain = 'https://api.redcore.cn'
			
			// 测试地址
			//domain = 'http://api.enterplorer.net'
        }
        HOMEPROXY.login(domain, _this.userid.val(), _this.password.val());
        _this.loading();
    }

    var domainVer = function () {
        // if (!_this.domain.val()) {
        //     _this.errorTip('服务器地址输入不正确');
        //     return;
        // }
        step = 2;
        $('#company_domain').css('display', 'none');
        $('#company_userMessage').css('display', 'block');
        _this.errorTip('');
    }
    // 填入企业域名点击下一步
    $('#next-btn').click(function() {
        domainVer();
    });

    // 返回上一步添加企业域名
    $('#domain-back').click(function() {
        step = 1;
        _this.errorTip('');
        $('#company_domain').show();
        $('#company_userMessage').hide();
    })

    // 通过隐藏按钮来 设置 页面js和content_script的通信
    var getProxyEl = function () {
        return top.document.getElementById('btn_page_to_content_script');
    };
    // 在dom中写入数据
    window.__page_to_content_script__ = function (url, deviceId) {
        var proxy = getProxyEl();
        proxy.setAttribute('data-type', '__page_to_content_script__');
        proxy.setAttribute('data-url', url);
        proxy.setAttribute('data-deviceId', deviceId);
        proxy.click();
    };


    // 接收来自 content_script的 信息
    document.getElementById('btn_content_script_to_page').addEventListener('click', function (e) {
        var type = this.getAttribute('data-type');
        switch (type) {
            case '__content_script_to_page___':
                var flag = this.getAttribute('data-flag');
                // 扫码成功
                if (flag === 'scanned') {

                    $('#code_pack').hide();
                    $('#code_pack_qrcodeInvalid').hide();
                    $('#code_pack_qrcodeSuccess').show();
                }
                // 过期
                if (flag === 'timeout' || flag === 'cancelled') {
                    $('#code_pack').hide();
                    $('#code_pack_qrcodeSuccess').hide();
                    $('#code_pack_qrcodeInvalid').show();
                }
                break;
        }
    }, false);

    // 生成二维码
    var qrcode = new QRCode(document.getElementById("qrcode_img"), {
        width : 150,
        height : 150
    });
    // // 点击二维码
    $('#QRcode').click(function() {
        // if (!_this.domain.val()) {
        //     _this.errorTip('设置服务器地址');
        //     return;
        // }
        serverAddress = _this.domain.val() || 'api.redcore.cn';
        // 建立长连接
        window.__page_to_content_script__('http://' + serverAddress, deviceId);
        $('#code_pack_qrcodeInvalid').hide();
        $('#code_pack_qrcodeSuccess').hide();
        $('#code_pack').css('display', 'flex');
        _this.errorTip('');
        // 获取二维码
        $.ajax({
            method: "GET",
            url: "http://" + serverAddress + '/client/v3/push/qrlogin/qrcode?deviceId=' + deviceId,
            success: function(res) {
                var resData = JSON.parse(res);
                if (resData.qrcode) {
                    qrcode.makeCode(resData.qrcode);
                    $('#qrcode_img').attr("title"," ");
                } else {
                    _this.errorTip('获取二维码失败 ');
                }
            },
            error: function(err) {
                _this.errorTip('获取二维码失败 ');
                for (var i = 0; i < $("#qrcode_img>img").length; i ++) {
                    $("#qrcode_img>img")[i].style.display = 'none'
                }
            }
        });
        $('#company_QRcode').css('display', 'block');
        $('#company_userMessage').css('display', 'none');
        $('#company_domain').css('display', 'none');
    });

    // 刷新重新获取二维码
    $('#refresh_qrcode').click(function() {
        $.ajax({
            method: "GET",
            url: "http://" + serverAddress + '/client/v3/push/qrlogin/qrcode?deviceId=' + deviceId,
            success: function(res) {
				var resData = JSON.parse(res);
                if (resData.qrcode) {
                    qrcode.makeCode(resData.qrcode);
                    $('#qrcode_img').attr("title"," ");
					$('#code_pack_qrcodeInvalid').hide();
                    $('#code_pack_qrcodeSuccess').hide();
                    $('#code_pack').css('display', 'flex');
                } else {
                    _this.errorTip('获取二维码失败 ');
                }
            },
            error: function(err) {
                _this.errorTip('获取二维码失败 ');
            }
        });
    });

    $('#back_login').click(function() {
        _this.errorTip('');
        $('#company_QRcode').css('display', 'none');
        $('#company_domain').css('display', 'none');
        $('#company_userMessage').css('display', 'block');
    });


    $('#login-btn').click(function() {
      loginServer();
    });

    $('#code_pack').mousemove(function(e){
        $('#code_tip').show();
    });
    $('#code_pack').mouseleave(function(e){
        $('#code_tip').hide();
    });

    window.addEventListener('keydown', function (e) {
      if ($('#company_userMessage').css('display') === "block" && e.which == 13) {
          loginServer();
      }
        if ($('#company_domain').css('display') !== "none" && e.which == 13) {
            _this.errorTip('');
            domainVer();
        }
    })

    // show manager-server-address setting page
    var showServerSetting = function() {
      $('#form').hide();
      $('#server').show();
    }
    var hideServerSetting = function() {
      $('#form').show();
      $('#server').hide();
    }

    $('#auto-login-checkbox').change(function() {
      HOMEPROXY.setAutoLogin(this.checked)
    })

    $('#confirm-server-address').click(function() {
       HOMEPROXY.setManagerAddress(_this.address.val())
       hideServerSetting()
    })

    // show or hide server-address setting page
    $('#server-setting').click(function() {
        $('#company_domain').show();
        $('#company_userMessage').hide();
        _this.errorTip('');
    })
    // $('#server-back').click(function() {
    //  hideServerSetting()
    // })

    // 打开提示忘记密码弹框
    $('#openTip').click(function () {
        $('#forgetPassword').show();
    });
    $('#closeTip').click(function () {
        $('#forgetPassword').hide();
    });

}


// ++++++++++++++++++++
// LOGIN = new Login();
/**
 * handle loading status
 */
Login.prototype.removeLoading = function() {
  $('#login-btn').text('登 录');
  $('#login-btn').removeClass('login-button');
}
Login.prototype.loading = function() {
  $('#login-btn').text('登录中');
  $('#login-btn').addClass('login-button');
}

/**
 * set default value
 * @param {*} data 
 */
Login.prototype.setLoginInfo = function(data) {
  this.domain.val(data.domain)
  this.userid.val(data.userid)
}
Login.prototype.setAutoLoginStatus = function(data) {
  document.querySelector('#auto-login-checkbox').checked = data
}
Login.prototype.setManagerAddress = function(data) {
  $('#address').val(data);
}

/**
 * set error tip for user input
 * @param {string} error info
 */
Login.prototype.errorTip = function(data) {
    if (data) {
        $('#error-row').css('background', '#fee6e6');
        $('#error-tip').html("<img src=\"./imgs/error_icon.png\" alt=\"\">" + data);
    } else {
        $('#error-row').css('background', 'none');
        $('#error-tip').html('');
    }
}
>>>>>>> ffef05f456bc... Add untracked files after rebase

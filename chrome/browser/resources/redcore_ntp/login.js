/**
 * if login, directly show Nav page
 */
function switchPage(isLogin) {
  if (isLogin) {
    // 已经登录
    $('#content').empty();
    $('#content').append(`<div class="nav" id="nav">
      <div class="title"> <img alt="logo" class="logo" id="logo" src="./imgs/defaultLogo.png"/> </div>
        <div class="search-box" id="search-box">
          <input id="search-input" class="field field-block" placeholder="搜索 web" />
          <span id="search-btn" title="search"></span>
        </div>
        <div id="homePage">
          <div class="company-title" id="company-title"> <h1 id="title"></h1>
          </div>
          <div class="nav-tabs" id="tab-wrap"> </div>
          <div class="peeler-pack"> <img id="skin_peeler" src="./imgs/appearancel_icon.png" alt="">
            <p>换 肤</p>
          </div>
          <div id="peeler-nav" class="peeler-nav mini-peeler-nav">
            <div class="peeler-container">
              <div class="peeler-title"> <h3>外观设置</h3> <img id="peeler_close" src="./imgs/close_icon.png" alt="">
            </div>
            <div class="peeler-content"> <h4>主页背景</h4>
              <ul id="list-unstyled">
                <li> <img class="bg-li" id="bg-1" src="./imgs/background/abb-1.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>迷夜（默认）</p>
                </li>
                <li> <img class="bg-li" id="bg-2" src="./imgs/background/abb-2.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>星河</p>
                </li>
                <li> <img class="bg-li" id="bg-3" src="./imgs/background/abb-3.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>绮梦</p>
                </li>
                <li> <img class="bg-li" id="bg-4" src="./imgs/background/abb-4.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>莱茵</p>
                </li>
                <li> <img class="bg-li" id="bg-5" src="./imgs/background/abb-5.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>极光</p>
                </li>
                <li> <img class="bg-li" id="bg-6" src="./imgs/background/abb-6.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>都市</p>
                </li>
                <li> <img class="bg-li" id="bg-7" src="./imgs/background/abb-7.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>峡谷</p>
                </li>
                <li> <img class="bg-li" id="bg-8" src="./imgs/background/abb-8.png"> <img class="selected" src="./imgs/selected_icon.png">
                  <p>星轨</p>
                </li>
              </ul>
            </div>
          </div>
        </div>
      </div>
      <div class="market" id="tab-wrap-market">
        <div class="market-banner"></div>
        <div class="back-pack"> <img class="back" id="back_app" src="./imgs/back_normal_icon.png" alt="">
          <p>返回</p>
        </div>
        <div class="market-container" id="market_container"> </div>
      </div>
    </div>`);
    var bgId = localStorage.getItem('background') || 'bg-1';
    for (var i = 0; i < $('.bg-li').length; i++) {
      if ($('.bg-li')[i].id == bgId) {
        $('#' + bgId).next().show();
        break;
      }
    }
    $('#nav').css("background-image", "url(./imgs/background/" + bgId + ".png)");
    // 换肤开始——————————————————————————————————————————————————————
    $('#skin_peeler, #peeler_close').on('click', function() {
      if (!$('.peeler-nav').hasClass('mini-peeler-nav')) {
        $('.peeler-nav').addClass('mini-peeler-nav');
        $('#skin_peeler').show();
        $('#peeler_close').hide();
      } else {
        $('.peeler-nav').removeClass('mini-peeler-nav');
        $('#skin_peeler').hide();
        $('#peeler_close').show();
      }
    });

    $('#list-unstyled').on('click', '.bg-li', function() {
      var thisID = ''
      for (var i = 0; i < $('.selected').length; i++) {
        $('.selected').hide();
      }
      $(this).next().show();
      thisID = $(this).attr("id");

      $('#nav').css("background-image", "url(./imgs/background/" + thisID + ".png)");
      localStorage.setItem('background', thisID);
    })
    // 换肤结束——————————————————————————————————————————————————————
    search();

  } else {
    $('#content').empty();
    $('#content').append(`<div id="login" class="login">
    <div class="wrap">
      <div class="wrap-left"></div>
      <div class="form" id="form">
        <div class="cut_off_line"></div>
        <div class="company_domain" id="company_domain">
          <h2 class="set_server">服务器地址</h2>
          <input id="domain" type="text" class="field field-block" 
                 placeholder="https://ysp.redcore.cn" pattern="\S+" tip="" />
          <p>忘记服务器地址？请联系您的企业管理员获取。</p>
          <button class="button block" id="next-btn" name="">确定</button>
        </div>
        <div class="company_userMessage" id="company_userMessage">
          <h2 id="QRcode">扫码登录</h2>
          <h2>账号登录</h2>
          <p class="userName-pack">
            <img src="./imgs/login/user_icon.png">
            <input id="userid" type="text" class="field field-block" placeholder="请输入您的邮箱或手机号" pattern="\S+" tip="" />
          </p>
          <p class="password-pack">
            <img src="./imgs/login/password_icon.png">
            <input id="password" type="password" class="field field-block" placeholder="请输入密码" pattern="\S+" tip="" />
          </p>
          <div class="setting setting-server">
            <span id="server-setting" class="link"> 设置服务器地址 </span>
          </div>
          <div id="error-row" class="row">
            <span class="error-tip" id="error-tip"></span>
          </div>
          <button class="button block" id="login-btn">登 录</button>
          <div class="setting">
            <label class="checkbox-label">
              <input type="checkbox" id="auto-login-checkbox" />
              <span class="checkbox"></span>
              <span>自动登录</span>
            </label>
            <span class="forget" id="openTip">忘记密码？</span>
          </div>
        </div>
        <div class="company_QRcode" id="company_QRcode">
          <h2 id="back_login">账号登录</h2>
          <h2 class="QRcode_login">扫码登录</h2>
          <div id="code_pack">
            <div id="qrcode_img"></div>
            <div id="code_tip" style="">
              <img src="./imgs/login/scan_blue.png" alt=""> </div>
            </div>
            <div id="code_pack_qrcodeInvalid" style="display: block">
              <p>二维码已失效</p> <button class="button block refresh" id="refresh_qrcode">请点击刷新</button>
            </div>
            <div id="code_pack_qrcodeSuccess" style="display: none"> <img src="./imgs/login/qrcodeSuccess.png" alt="">
              <p>扫描成功</p>
              <p>请在手机上确认登录</p>
            </div>
            <div id="error-row1" class="row code-row"> <span class="error-tip" id="error-tip1"></span> </div>
            <p id="app_tip">请使用 Mobile 扫描二维码</p>
            <p class="app_group"> <img src="./imgs/login/Group.png" alt=""> </p>
              <p class="download"> <a href="https://www.redcore.cn/download/index.html">下载 Redcore mobile</a> </p>
          </div>
        </div>
        <div class="forgetPassword" id="forgetPassword">
          <p> 忘记密码? 请联系您的企业管理员， </p>
          <p> 企业管理员可在控制台修改您的密码。 </p>
          <p class="bottom"> <button id="closeTip">确定</button> </p>
        </div>
      </div>
    </div>`);
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
  var loginServer = function() {

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
      if (!(domain.startsWith('http://') || domain.startsWith('https://'))) {
        _this.errorTip('请输入正确的服务器地址')
        return
      } else {
        let url = new URL(domain)
        let regDomain = /[a-za-z0-9][\w\u4E00-\u9FA5]*(\.[\w\u4E00-\u9FA5]+)+/
        let regIp = /^(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|[1-9])(\.(1\d{2}|2[0-4]\d|25[0-5]|[1-9]\d|\d)){3}$/
        if (!(regDomain.test(url.host) || regIp.test(url.host))) {
          _this.errorTip('请输入正确的服务器地址')
          return
        }
      }
    } else {
      // 正式地址
      domain = 'https://ysp.redcore.cn'

      // 测试地址
      //domain = 'http://api.enterplorer.net'
    }
    HOMEPROXY.login(domain, _this.userid.val(), _this.password.val());
    _this.loading();
  }

  var domainVer = function() {
    step = 2;
    $('#company_domain').hide();
    $('#company_userMessage').show();
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
  var getProxyEl = function() {
    return top.document.getElementById('btn_page_to_content_script');
  };
  // 在dom中写入数据
  window.__page_to_content_script__ = function(url, deviceId) {
    var proxy = getProxyEl();
    proxy.setAttribute('data-type', '__page_to_content_script__');
    proxy.setAttribute('data-url', url);
    proxy.setAttribute('data-deviceId', deviceId);
    proxy.click();
  };


  // 接收来自 content_script的 信息
  document.getElementById('btn_content_script_to_page').addEventListener('click', function(e) {
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
    width: 150,
    height: 150
  });
  // 点击二维码
  $('#QRcode').click(function() {
    $('#code_pack').css('visibility', 'hidden');
    serverAddress = _this.domain.val() || 'https://ysp.redcore.cn';
    // 建立长连接
    window.__page_to_content_script__(serverAddress, deviceId);
    $('#code_pack_qrcodeInvalid').hide();
    $('#code_pack_qrcodeSuccess').hide();
    _this.errorTip('');
    // 获取二维码
    $.ajax({
      method: "GET",
      url: serverAddress + '/client/v3/push/qrlogin/qrcode?deviceId=' + deviceId,
      success: function(res) {
        $('#code_pack').css('display', 'flex');
        $('#code_pack').css('visibility', 'visible');
        var resData = JSON.parse(res);
        if (resData.qrcode) {
          qrcode.makeCode(resData.qrcode);
          $('#qrcode_img').attr("title", " ");
        } else {
          _this.errorTip('获取二维码失败 ');
        }
      },
      error: function(err) {
        _this.errorTip('获取二维码失败 ');
        for (var i = 0; i < $("#qrcode_img>img").length; i++) {
          $("#qrcode_img>img")[i].style.display = 'none'
        }
      }
    });
    $('#company_QRcode').show();
    $('#company_userMessage').hide();
    $('#company_domain').hide();
  });

  // 刷新重新获取二维码
  $('#refresh_qrcode').click(function() {
    $.ajax({
      method: "GET",
      url: serverAddress + '/client/v3/push/qrlogin/qrcode?deviceId=' + deviceId,
      success: function(res) {
        var resData = JSON.parse(res);
        if (resData.qrcode) {
          qrcode.makeCode(resData.qrcode);
          $('#qrcode_img').attr("title", " ");
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
    $('#company_QRcode').hide();
    $('#company_domain').hide();
    $('#company_userMessage').show();
  });


  $('#login-btn').click(function() {
    loginServer();
  });

  $('#code_pack').mousemove(function(e) {
    $('#code_tip').show();
  });
  $('#code_pack').mouseleave(function(e) {
    $('#code_tip').hide();
  });

  window.addEventListener('keydown', function(e) {
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
  $('#openTip').click(function() {
    $('#forgetPassword').show();
  });
  $('#closeTip').click(function() {
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
    $('#error-row, #error-row1').css('background', '#fee6e6');
    $('#error-tip, #error-tip1').html("<img src=\"./imgs/error_icon.png\" alt=\"\">" + data);
  } else {
    $('#error-row, #error-row1').css('background', 'none');
    $('#error-tip, #error-tip1').html('');
  }
}

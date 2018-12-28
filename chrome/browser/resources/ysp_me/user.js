// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const config = {
  holder: 'rdc_user',
  login_status: '',
  pop_psw_nopermit: `<div class="rdc-masker-content rdc-pos-mm" style="padding-bottom:60px">
      <h5>修改密码</h5>
      <p>很抱歉，非红芯SDP控制台本地认证用户，无权修改密码。</p>
      <div class="rdc-ctl" style="bottom: 20px; right:20px; top: unset"> 
          <button id="btn_rdc_tocancel" class="rdc-btn primary"> 确认 </button> 
        </div> 
    </div>`,
  pop_psw_success: `<div class="rdc-masker-content rdc-pos-mm" style="padding-bottom:60px">
      <h5>修改密码</h5>
      <p>密码修改成功，浏览器将自动退出，请重新登录。</p>
      <div class="rdc-ctl" style="bottom: 20px; right:20px; top: unset"> 
          <button id="btn_rdc_toLoginout" class="rdc-btn primary"> 确认 </button> 
        </div> 
    </div>`,
  pop_modify_psw: `<div id="rdc_pop_modify_psw" class="rdc-masker-fix">
    <div class="rdc-masker-content rdc-pos-mm">
      <h5>修改密码</h5>
      <div class="rdc-content rdc-pop">
        <div class="rdc-form-row">
          <label>请输入原密码</label> <input type="password" placeholder="请输入原密码" name="originalPSW">
        </div>
        <div class="rdc-form-row">
          <label>请输入新密码</label> <input type="password" placeholder="6-14位，至少包含英文字母及数字" name="newPSW">
        </div>
        <div class="rdc-form-row">
          <label>确认新密码</label> <input type="password" placeholder="6-14位，至少包含英文字母及数字" name="newPSWConfirm">
        </div>
        <div class="rdc-form-valid">
          <span></span>
        </div>
        <div class="rdc-ctl-br">
          <button id="modify_psw_cancel" class="rdc-btn">取消</button>
          <button id="modify_psw_ok" class="rdc-btn primary">确定</button>
        </div>
      </div>
    </div>
  </div>`
}
const _ins = Symbol.for('UserIns')
const req_getUserInfo = Symbol.for('UserInfo')
const init_rdc_user_html = Symbol.for('init_rdc_user_html')
const init_evt = Symbol.for('UserInitEvt')

class User {
  constructor(opt) {
    if (opt) {
      Object.assign(config, opt);
    }
    this.holder = document.getElementById(config.holder);
    this[req_getUserInfo]();
  }

  static get ins() {
    if (this[_ins] == null || !this[_ins]) {
      this[_ins] = new User()
    }
    return this[_ins]
  }

  /**
   * 从内核获取用户信息
   */
  [req_getUserInfo]() {
    chrome.send("getUserInfo");
  }

  [init_rdc_user_html](user) {
    if (user.avatarPath) {
      $('#rdc_user_head_img').attr('src', user.avatarPath)
    }
    $('#rdc_user_head_info').html(`<p>${user.headName || ''}</p> <p>${user.email || ''} </p>`)
    $('#rdc_user_head_info').next().html(`<button class="rdc-btn-icon on">
      <span class="rdc-shadow"></span>
      <span class="rdc-caret-arrow rdc-pos-mm"></span>
    </button>`)
    $('#rdc_user_info_holder .rdc-content').remove();
    $('#rdc_user_info_holder').append(`<div class="rdc-content"> 
      <div class="rdc-form-row"> 
        <label>手机</label> <span>${user.mobile || ''}</span> 
      </div> 
      <div class="rdc-form-row"> 
        <label>公司</label> <span>${user.companyName || ''}</span> 
      </div> 
      <div class="rdc-form-row"> 
        <label>部门</label> <span>${user.department || ''}</span> 
      </div> 
      <div class="rdc-form-row"> 
        <label>职位</label> <span>${user.title || ''}</span> 
      </div> 
      <div class="rdc-form-valid"> 
        <span></span> 
      </div> 
      <div class="rdc-ctl-br"> 
        <button id="btn_rdc_loginout" class="rdc-btn">退出登录</button> 
        <button id="btn_rdc_modify_psw" class="rdc-btn primary">修改密码</button> 
      </div> 
    </div> `)
  }

  /**
   * 初始化事件注册
   */
  [init_evt]() {
    let that = this;
    $('#btn_rdc_login').click(function() {
      top.location.href = "chrome://newtab"
    })
    $('#btn_rdc_loginout').click(function() {
      chrome.send('logout');
    })
    $('#btn_rdc_modify_psw').click(function() {
      chrome.send('checkIfCanModifyPassword')
    })
  }

  static getUserInfoFinish(userInfo) {
    if (userInfo) {
      let user = JSON.parse(userInfo);
      this[_ins][init_rdc_user_html](user);
      // 初始化锁屏设置
      this.screenSet = new ScreenSet();
      // 初始化用户设备
      this.rdc_user_device = new Device({ user: { name: user.headName } });
    }
    this[_ins][init_evt]()
  }

  static loginFinish(data) {
  }

  static logoutFinish() {
    top.location.reload()
  }

  static getLockScreenTimeFinish(data) {
    this.screenSet.setLockTime(data);
  }

  static getDeviceInfoFinish(data) {
    if (typeof data === 'string') {
      data = JSON.parse(data)
    }
    if (data.errCode === '0') {
      this.rdc_user_device.initDOM(data)
    }
  }

  /**
   * @param  {[type]}
   * @return {[type]}
   */
  static modifyPINCodeFinish(data) {
    switch (data) {
      case 0:
        {
          // 成功
          this.screenSet.modifyPIN0();
          break;
        }
      case 1:
        {
          // 某项为空
          this.screenSet.modifyPIN1();
          break;
        }
      case 2:
        {
          // 原始锁屏密码不正确
          this.screenSet.modifyPIN2();
          break;
        }
      case 3:
        {
          // 锁屏密码和确认锁屏密码不一致
          this.screenSet.modifyPIN3();
          break;
        }
      case 4:
        {
          // 锁屏密码不是6位数字
          this.screenSet.modifyPIN4();
          break;
        }
      case 5:
        {
          // 原始锁屏密码和新锁屏密码一致
          this.screenSet.modifyPIN5();
          break;
        }
    }
  }

  /**
   * @param  {[string]} data [修改密码回调函数]
   * @return {[none]}
   */
  static modifyPasswordFinish(data) {
    switch (data) {
      case '0':
        {
          // 成功
          $('#rdc_pop_modify_psw').html(config.pop_psw_success);
          $('#btn_rdc_toLoginout').click(function() {
            chrome.send('logout');
          })
          break;
        }
      case 'E5001':
        {
          $('#rdc_pop_modify_psw').find('.rdc-form-valid span').html('请重试')
          break;
        }
      case 'E5002':
        {
          // 原始密码错误
          $('#rdc_pop_modify_psw').find('.rdc-form-valid span').html('原始密码错误')
          break;
        }
      case 'E5003':
        {
          // 抱歉，您非红芯SDP控制台本地认证用户，无权修改
          $('#rdc_pop_modify_psw').html(config.pop_psw_nopermit);
          $('#btn_rdc_tocancel').click(function() {
            $('#rdc_pop_modify_psw').remove()
          })
          break;
        }
    }
  }

  /**
   * @param  {[type]}
   * @return {[string]} 
   * 正确返回值： {"errCode":"0"}
   */
  static deleteDeviceFinish(data) {
    data = JSON.parse(data)
    if (data.errCode === '0') {
      this.rdc_user_device.getDeviceInfo()
    }
  }

  static checkIfCanModifyPasswordFinish(data) {
    if (data === 'true') {
      $(document.body).append($(config.pop_modify_psw))
      $('#rdc_pop_modify_psw').show()
      $('#modify_psw_cancel').click(function() {
        $('#rdc_pop_modify_psw').hide().remove();
      })
      $('#modify_psw_ok').click(function() {
        let op = $('input[name="originalPSW"]').val();
        let np = $('input[name="newPSW"]').val();
        let cp = $('input[name="newPSWConfirm"]').val();
        if (!op || !np || !cp) {
          return false;
        }
        if (np != cp) {
          return false;
        }
        if (!/^(?=.*[a-zA-Z]+)(?=.*\d)[^\s]{6,16}$/.test(np)) {
          $('#rdc_pop_modify_psw').find('.rdc-form-valid span').html('6-14位，至少包含英文字母及数字')
          return false;
        }
        chrome.send('modifyPassword', [op, np])
      })
    } else {
      $(document.body).append($(config.pop_modify_psw))
      $('#rdc_pop_modify_psw').html(config.pop_psw_nopermit).show()
      $('#btn_rdc_tocancel').click(function() {
        $('#rdc_pop_modify_psw').remove()
      })
    }
  }

  static uploadAvatarFinish(res) {
    if (typeof res === 'string') {
      res = JSON.parse(res);
    }
    // TODO: UI 没有设计如果上传失败的情况
    if (res.errCode === '0') {
      $('#rdc_user_head_img').attr('src', res.data.url);
    }
  }
}

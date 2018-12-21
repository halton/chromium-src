// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const cfg = {
  holder: 'rdc_screen_set',
  pop_pin_error: `<div id="rdc_pop_modify_psw" class="rdc-masker-fix">
    <div class="rdc-masker-content rdc-pos-mm">
      <h5>修改锁屏密码</h5>
      <p>{error}</p>
  </div>`,
  pop_pin_success: `<div id="rdc_pop_modify_psw" class="rdc-masker-fix">
    <div class="rdc-masker-content rdc-pos-mm">
      <h5>修改锁屏密码</h5>
      <p>锁屏密码修改成功，浏览器将自动退出，请重新登录。</p>
    </div>
  </div>`,
  tmpl_pop: `<div id="rdc_pop_modify_pin" class="rdc-masker-fix">
    <div class="rdc-masker-content rdc-pos-mm">
      <h5>修改锁屏密码</h5>
      <div id="frm_rdc_modify_pin" class="rdc-content rdc-pop">
        <div class="rdc-form-row">
          <label>请输入原锁屏密码</label> <input id="txt_rdc_pin_origin" type="password" name="originalPIN">
        </div>
        <div class="rdc-form-row">
          <label>请输入新锁屏密码</label> <input id="txt_rdc_pin_new" type="password" name="newPIN">
        </div>
        <div class="rdc-form-row">
          <label>确认新锁屏密码</label> <input id="txt_rdc_pin_confirm" type="password" name="newPINConfirm">
        </div>
        <div class="rdc-form-valid">
          <span></span>
        </div>
        <div class="rdc-ctl-br">
          <button id="btn_rdc_modify_pin_cancel" class="rdc-btn">取消</button>
          <button id="btn_rdc_modify_pin_ok" class="rdc-btn primary">确定</button>
        </div>
      </div>
    </div>
  </div>`
}

const screen_set = Symbol.for('screen_set_ins');
const req_getLockTime = Symbol.for('LockTime');
class ScreenSet {
  constructor(opt) {
    if (opt) {
      Object.assign(cfg, opt)
    }
    this.holder = document.getElementById(cfg.holder)
    this[req_getLockTime]();
    this.initEvt();
  }

  static get ins() {
    if (this[screen_set] == null || !this[screen_set]) {
      this[screen_set] = new ScreenSet()
    }
    return this[screen_set]
  }

  [req_getLockTime]() {
    chrome.send('getLockScreenTime')
  }

  setLockTime(data) {
    $('#rdc_srceen_lock_time').html(`${data} 分钟`)
  }

  initEvt() {
    let that = this;
    $('#btn_rdc_showPop').click(function() {
      $(document.body).append($(cfg.tmpl_pop))
      $('#rdc_pop_modify_pin').show();

      $('#btn_rdc_modify_pin_cancel').click(function() {
        $('#rdc_pop_modify_pin').hide().remove();
        return false;
      })

      $('#btn_rdc_modify_pin_ok').click(function() {
        let origin = $('#txt_rdc_pin_origin').val()
        let newset = $('#txt_rdc_pin_new').val()
        let confirm = $('#txt_rdc_pin_confirm').val()
        if (!origin || !newset || !confirm) {
          that.modifyPIN1()
          return false
        }
        if (newset != confirm) {
          that.modifyPIN3()
          return false
        }
        if (!/^\d{6}$/.test(newset)) {
          that.modifyPIN4()
          return false
        }
        let elValid = $('#rdc_pop_modify_pin').find('.rdc-form-valid')
        elValid.removeClass('error');
        elValid.find('span').html('')
        console.log(`param:${origin},${newset},${confirm}`)
        chrome.send('modifyPINCode', [origin, newset, confirm])
        return false;
      })
      return false;
    });
  }

  modifyPIN0() {
    $('#rdc_pop_modify_pin').remove();
  }

  modifyPIN1() {
    let elValid = $('#rdc_pop_modify_pin').find('.rdc-form-valid')
    elValid.addClass('error');
    elValid.find('span').html('锁屏密码不能为空！')
  }

  modifyPIN2() {
    let elValid = $('#rdc_pop_modify_pin').find('.rdc-form-valid')
    elValid.addClass('error');
    elValid.find('span').html('原始锁屏密码不正确')
  }
  modifyPIN3() {
    let elValid = $('#rdc_pop_modify_pin').find('.rdc-form-valid')
    elValid.addClass('error');
    elValid.find('span').html('锁屏密码和确认锁屏密码不一致')
  }
  modifyPIN4() {
    let elValid = $('#rdc_pop_modify_pin').find('.rdc-form-valid')
    elValid.addClass('error');
    elValid.find('span').html('锁屏密码不是6位数字')
  }
  modifyPIN5() {
    let elValid = $('#rdc_pop_modify_pin').find('.rdc-form-valid')
    elValid.addClass('error');
    elValid.find('span').html('原始锁屏密码和新锁屏密码一致')
  }
}

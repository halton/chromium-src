// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const device_ins = Symbol.for('rdc_device_ins')
const rdc_device_dom_init = Symbol.for('rdc_device_dom_init')
const rdc_device_evt_init = Symbol.for('rdc_device_evt_init')
class Device {
  constructor(opt) {
    this.cfg = {
      holder: $('#rdc_device_set'),
      arrDevices: []
    }
    Object.assign(this.cfg, opt)
    this.getDeviceInfo()
    this.initEvt()
  }

  static get ins() {
    if (!this[device_ins] || this[device_ins] === null) {
      this[device_ins] = new Device()
    }
    return this[device_ins]
  }

  discernDevice(str) {
    return /windows/i.test(str) ? './imgs/windows_pc.png' :
           /Android/i.test(str) ? './imgs/android.png' :
           /os x/i.test(str) ? './imgs/macbook.png' :
           /iphone|ios/i.test(str) ? './imgs/iphone.png' :
           /ipad/i.test(str) ? './imgs/iPad.png' :
           /linux/i.test(str) ? './imgs/linux.png' : './imgs/mac.png'
  }

  initDOM(data) {
    this.cfg.arrDevices = [].concat(data.data)
    this.cfg.holder.html(`<h5>设备管理</h5> ${data.data.map(item => 
      `<div class="rdc-info device">
      <div class="rdc-head">
        <div class="img" data-imgSrc="${this.discernDevice(item.operatingSystem)}">
          </div>
          <div class="info">
            <p>${this.cfg.user.name}</p>
            <p>${item.model}</p>
          </div>
          <div class="rdc-ctl">
            <button class="rdc-btn-icon on">
              <span class="rdc-shadow"></span>
              <span class="rdc-caret-arrow rdc-pos-mm"></span>
            </button>
          </div>
        </div>
        <div class="rdc-content">
          <div class="rdc-form-row">
            <label>操作系统</label>
            <span>${item.operatingSystem}</span>
          </div>
          <div class="rdc-form-row">
            <label>设备ID</label>
            <span>${item.id}</span>
          </div>
          <div class="rdc-form-row">
            <label>客户端版本</label>
            <span>${item.clientVersion}</span>
          </div>
          <div class="rdc-form-row">
            <label>上次登录时间</label>
            <span>${Rdc.formateTime(item.lastLoginTime) || ''}</span>
          </div>
          <div class="rdc-form-row">
            <label>上次登录位置</label>
            <span>${item.lastLoginPos || ''}</span>
          </div>
          <div class="rdc-ctl-br">
            <button class="rdc-btn" data-id="">删除设备</button>
          </div>
        </div>
      </div>`).join(' ')} `);
    this.cfg.holder.find('.img').each(function(idx, item) {
      let img = new Image()
      img.src = $(item).attr('data-imgSrc')
      $(item).append(img)
      $(item).attr('data-imgSrc', '')
    })
  }

  initEvt() {
    let that = this;
    this.cfg.holder.delegate('.rdc-btn', 'click', function() {
      let idx = $(this).parents('.device').index() - 1
      chrome.send('deleteDevice', [that.cfg.arrDevices[idx].id])
      return false;
    })
  }

  getDeviceInfo() {
    chrome.send('getDeviceInfo')
  }
}

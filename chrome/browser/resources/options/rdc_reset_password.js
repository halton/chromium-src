// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

cr.define('options', function () {
    /** @const */ var Page = cr.ui.pageManager.Page;
    /** @const */ var PageManager = cr.ui.pageManager.PageManager;

  /** @const */ var ArrayDataModel = cr.ui.ArrayDataModel;
  /** @const */ var SettingsDialog = options.SettingsDialog;

  /**
   * ResetPasswordOverlay class
   * Encapsulated handling of the 'change password in redcore' overlay page.
   * @constructor
   * @extends {options.SettingsDialog}
   */
  function ResetPasswordOverlay() {
    SettingsDialog.call(this, 'resetpsw',
        '修改密码',
        'rdc_reset_password',
        assertInstanceof($('rdcRestpsw_confirm'), HTMLButtonElement),
        assertInstanceof($('rdcRestpsw_cancel'), HTMLButtonElement));
  };

  cr.addSingletonGetter(ResetPasswordOverlay);

  ResetPasswordOverlay.prototype = {
    __proto__: SettingsDialog.prototype,

    startup_pages_pref_: {
      'name': 'session.startup_urls',
      'disabled': false
    },

    /** @override */
    initializePage: function() {
      // notify browser to return data
        SettingsDialog.prototype.initializePage.call(this);
        //this.afterInit();
        this.rdc_psw_model = {
            oldPassword: '',
            newPassword: '',
            newPassword2: ''
        };
        var self = this;
        $('rdcRestpsw_complete').addEventListener('click', function () {
            SettingsDialog.prototype.handleCancel.call(this);
            self.restore();
            chrome.send('logout');
        });

        $('rdcRestpsw_fail').addEventListener('click', function () {
            self.restore();
            SettingsDialog.prototype.handleConfirm.call(this);
        });

        $('txt_rdc_psw_origin').addEventListener('input', function () {
            self.validOrigin(this.value);
        });
        $('txt_rdc_psw_new').addEventListener('input', function () {
            self.validPsw(this.value);
        });
        $('txt_rdc_psw_new2').addEventListener('input', function () {
            self.validPsw2($('txt_rdc_psw_new').value, this.value);
        });
    },

    restore: function () {
        $('rdc_reset_frm').style.display = 'block';
        $('rdc_reset_success').style.display = 'none';
        $('rdc_reset_fail').style.display = 'none';
        $('rdc_reset_nopermit').style.display = 'none';

        var el_valid_origin = $('sp_rdc_psw_origin');
        el_valid_origin.innerHTML = '';
        el_valid_origin.classList.remove('on');

        var el_valid_psw = $('sp_rdc_psw_now');
        el_valid_psw.innerHTML = '';
        el_valid_psw.classList.remove('on');

        var el_psw_now2 = $('sp_rdc_psw_now2');
        el_psw_now2.innerHTML = '';
        el_psw_now2.classList.remove('on');
        $('txt_rdc_psw_origin').value = '';
        $('txt_rdc_psw_new').value = '';
        $('txt_rdc_psw_new2').value = '';
    },

    //afterInit: function () {
    //    var self = this;
    //    var el_rdc_psw_origin = $('txt_rdc_psw_origin');
    //    var el_rdc_psw_new = $('txt_rdc_psw_new');
    //    var el_rdc_psw_new2 = $('txt_rdc_psw_new2');

    //    el_rdc_psw_origin.addEventListener(
    //        'change', this.standardRangeChanged_.bind(this, el_rdc_psw_origin));
    //    el_rdc_psw_origin.addEventListener(
    //        'input', this.standardRangeChanged_.bind(this, el_rdc_psw_origin));

    //    el_rdc_psw_new.addEventListener(
    //        'change', this.standardRangeChanged_.bind(this, el_rdc_psw_new));
    //    el_rdc_psw_new.addEventListener(
    //        'input', this.standardRangeChanged_.bind(this, el_rdc_psw_new));

    //    el_rdc_psw_new2.addEventListener(
    //        'change', this.standardRangeChanged_.bind(this, el_rdc_psw_new2));
    //    el_rdc_psw_new2.addEventListener(
    //        'input', this.standardRangeChanged_.bind(this, el_rdc_psw_new2));
    //},

    handleChange_: function (el, event) {
        //console.log(el.value);
        if (el.id == 'txt_rdc_psw_origin') {
            this.rdc_psw_model.oldPassword = el.value;
        }
        if (el.id == 'txt_rdc_psw_new') {
            this.rdc_psw_model.newPassword = el.value;
        }
        if (el.id == 'txt_rdc_psw_new2') {
            this.rdc_psw_model.newPassword2 = el.value;
        }
    },

    initData: function(d) {
      //console.log(d);
    },

    validOrigin: function (v) {
        var el_valid_origin = $('sp_rdc_psw_origin');
        if (v == '') {
            el_valid_origin.innerHTML = '原始密码不能为空';
            el_valid_origin.classList.add('on');
            $('txt_rdc_psw_origin').focus();
        } else {
            el_valid_origin.innerHTML = '';
            el_valid_origin.classList.remove('on');
            return true;
        }
    },
    validPsw: function (v) {
        var el_valid_psw = $('sp_rdc_psw_now');
        if (!/\d+[a-zA-Z]+|[a-zA-Z]+\d/.test(v) || v.length < 6 || v.length > 14) {
            el_valid_psw.innerHTML = '6-14位,包含字母和数字';
            el_valid_psw.classList.add('on');
            $('txt_rdc_psw_new').focus();
        } else {
            el_valid_psw.innerHTML = '';
            el_valid_psw.classList.remove('on');
            return true;
        }
    },
    validPsw2: function (v, v2) {
        var el_psw_now2 = $('sp_rdc_psw_now2');
        if (v != v2) {
            el_psw_now2.innerHTML = '两次密码输入不一致';
            el_psw_now2.classList.add('on');
            $('txt_rdc_psw_new2').focus();
        } else {
            el_psw_now2.innerHTML = '';
            el_psw_now2.classList.remove('on');
            return true;
        }
    },
    // overwrite the confirm button event.
    handleConfirm: function () {
        //SettingsDialog.prototype.handleConfirm.call(this);
        var oldPassword = $('txt_rdc_psw_origin').value;
        var newPassword = $('txt_rdc_psw_new').value;
        var newPassword2 = $('txt_rdc_psw_new2').value;
        oldPassword = oldPassword.replace(/\s/g, '');
        newPassword = newPassword.replace(/\s/g, '');
        newPassword2 = newPassword2.replace(/\s/g, '');
        if (this.validOrigin(oldPassword) && this.validPsw(newPassword) && this.validPsw2(newPassword, newPassword2)) {
            chrome.send('modifyPassword', [oldPassword, newPassword]);
        }
    },


    modifyPasswordCallback_: function (d) {
        var errObj = {
            'E5001': '修改密码失败',
            'E5002': '原始密码错误',
            'E5003': '抱歉，您非红芯SDP控制台本地认证用户，无权修改',
            'E9999': '服务器内部错误，请联系管理员'
        };
        if (d === '0') {
            $('rdc_reset_frm').style.display = 'none';
            $('rdc_reset_success').style.display = 'block';
            //SettingsDialog.prototype.handleConfirm.call(this);
        } else if (d === 'E5001') {
            $('rdc_reset_frm').style.display = 'none';
            $('rdc_reset_fail').style.display = 'block';
        } else if (d === 'E5002') {
            var el_valid_origin = $('sp_rdc_psw_origin');
            el_valid_origin.innerHTML = errObj[d];
            el_valid_origin.classList.add('on');
            $('txt_rdc_psw_origin').focus();
        } else {
            $('rdc_reset_frm').style.display = 'none';
            $('rdc_reset_fail').style.display = 'block';
            $('rdc_reset_msg').innerHTML = errObj['E9999'];
        }
    }
  };

  // Forward public APIs to private implementations.
  cr.makePublic(ResetPasswordOverlay, [
    'modifyPasswordCallback'
  ]);

  // Export
  return {
    ResetPasswordOverlay: ResetPasswordOverlay
  };
});

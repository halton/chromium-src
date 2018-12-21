// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

$(function() {
  let user = User.ins;

  // 点击展示button
  $('.rdc-container-user').delegate('.rdc-btn-icon', 'click', function() {
    var $this = $(this)
    if (!$this.hasClass('on')) {
      $this.addClass('on');
    } else {
      $this.removeClass('on');
    }
    $this.find('.rdc-shadow').addClass('act');
    setTimeout(function() {
      $this.find('.rdc-shadow').removeClass('act')
    }, 300)
    let _el = $(this)
    if (_el.hasClass('on')) {
      _el.parents('.rdc-head').siblings('.rdc-content').slideDown();
    } else {
      _el.parents('.rdc-head').siblings('.rdc-content').slideUp();
    }
  });
})

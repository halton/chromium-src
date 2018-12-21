// Copyright 2018 The Redcore (Beijing) Technology Co.,Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

class Rdc {
  static formateTime(str) {
    if (!str) return ''
    let d = new Date(str);
  	let yyyy = d.getFullYear();
  	let MM = (d.getMonth() + 1).toString().padStart(2, '0');
  	let dd = d.getDate().toString().padStart(2, '0');
  	let HH = d.getHours().toString().padStart(2, '0');
  	let mm = d.getMinutes().toString().padStart(2, '0');
  	let ss = d.getSeconds().toString().padStart(2, '0');
    return `${yyyy}-${MM}-${dd} ${HH}:${mm}:${ss}`
  }
}

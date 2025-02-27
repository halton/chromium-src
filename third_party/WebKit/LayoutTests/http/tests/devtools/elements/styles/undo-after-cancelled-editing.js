// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(async function() {
  TestRunner.addResult(`Verifies that cancelling property value editing doesn't affect undo stack.\n`);
  await TestRunner.loadModule('elements_test_runner');
  await TestRunner.showPanel('elements');
  await TestRunner.loadHTML(`
      <style>
      #inspected {
      }
      </style>

      <div id="inspected">Text</div>
    `);

  var treeElement;
  TestRunner.runTestSuite([
    function selectNode(next) {
      ElementsTestRunner.selectNodeAndWaitForStyles('inspected', next);
    },

    function addNewProperty(next) {
      var section = ElementsTestRunner.firstMatchedStyleSection();
      var newProperty = section.addNewBlankProperty();
      newProperty.startEditing();
      newProperty.nameElement.textContent = 'color';
      newProperty.nameElement.dispatchEvent(TestRunner.createKeyEvent('Enter'));
      newProperty.valueElement.textContent = 'blue';
      newProperty.valueElement.dispatchEvent(TestRunner.createKeyEvent('Enter'));
      ElementsTestRunner.waitForStyleApplied(next);
    },

    function editProperty(next) {
      treeElement = ElementsTestRunner.getMatchedStylePropertyTreeItem('color');
      ElementsTestRunner.dumpSelectedElementStyles(true, false, true);
      treeElement.startEditing();
      treeElement.nameElement.textContent = 'color';
      treeElement.nameElement.dispatchEvent(TestRunner.createKeyEvent('Enter'));

      // Update incrementally, do not commit.
      treeElement.valueElement.textContent = 'red';
      treeElement.kickFreeFlowStyleEditForTest();
      ElementsTestRunner.waitForStyleApplied(next);
    },

    function cancelEditing(next) {
      treeElement.valueElement.dispatchEvent(TestRunner.createKeyEvent('Escape'));
      ElementsTestRunner.waitForStyleApplied(next);
    },

    function undoStyles(next) {
      ElementsTestRunner.dumpSelectedElementStyles(true, false, true);
      TestRunner.domModel.undo();
      ElementsTestRunner.waitForStyles('inspected', next, true);
    },

    function onUndoedProperty(next) {
      ElementsTestRunner.dumpSelectedElementStyles(true, false, true);
      next();
    }
  ]);
})();

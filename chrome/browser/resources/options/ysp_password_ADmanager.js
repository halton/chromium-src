// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
var appList = [];
cr.define('options', function() {
  /** @const */ var Page = cr.ui.pageManager.Page;
  /** @const */ var PageManager = cr.ui.pageManager.PageManager;
  /** @const */ var ArrayDataModel = cr.ui.ArrayDataModel;

  /////////////////////////////////////////////////////////////////////////////
  // PasswordADManager class:

  /**
   * Encapsulated handling of password and exceptions page.
   * @constructor
   * @extends {cr.ui.pageManager.Page}
   */
  function PasswordADManager() {
    this.activeNavTab = null;
    Page.call(this, 'SWA',
              loadTimeData.getString('passwordsPageTabTitle'),
              'password-ADmanager');
  }

  cr.addSingletonGetter(PasswordADManager);

  PasswordADManager.prototype = {
    __proto__: Page.prototype,

    /**
     * The saved passwords list.
     * @type {options.DeletableItemList}
     * @private
     */
    savedADPasswordsList_: null,

    /**
     * The password exceptions list.
     * @type {options.DeletableItemList}
     * @private
     */
    passwordExceptionsList_: null,

    /**
     * The timer id of the timer set on search query change events.
     * @type {number}
     * @private
     */
    queryDelayTimerId_: 0,

    /**
     * The most recent search query, or null if the query is empty.
     * @type {?string}
     * @private
     */
    lastQuery_: null,

    /** @override */
    initializePage: function() {
      Page.prototype.initializePage.call(this);

      $('auto-signin-block').hidden =
          !loadTimeData.getBoolean('enableCredentialManagerAPI');

      $('password-ADmanager-confirm').onclick = function() {
        PageManager.closeOverlay();
      };

      $('ADpassword-search-box').addEventListener('search',
          this.handleSearchQueryChange_.bind(this));

      // $('exceptions-learn-more').onclick = function() {
      //   chrome.send('coreOptionsUserMetricsAction',
      //               ['Options_PasswordManagerExceptionsLearnMore']);
      //   return true;  // Always follow the href
      // };

      this.createSavedADPasswordsList_();
    },

    /** @override */
    canShowPage: function() {
      return !(cr.isChromeOS && UIAccountTweaks.loggedInAsGuest());
    },

    /** @override */
    didShowPage: function() {
      // Updating the password lists may cause a blocking platform dialog pop up
      // (Mac, Linux), so we delay this operation until the page is shown.
      chrome.send('updateADPasswordLists');
      $('ADpassword-search-box').focus();
    },

    /**
     * Creates, decorates and initializes the saved passwords list.
     * @private
     */
    createSavedADPasswordsList_: function() {
      // 刷新后 获取密码列表
      var savedADPasswordsList = $('saved-ADpasswords-list');
      options.passwordADManager.ADPasswordsList.decorate(savedADPasswordsList);
      this.savedADPasswordsList_ = assertInstanceof(savedADPasswordsList,
          options.DeletableItemList);
      this.savedADPasswordsList_.autoExpands = true;
    },

    /**
     * Handles search query changes.
     * @param {!Event} e The event object.
     * @private
     */
    handleSearchQueryChange_: function(e) {
      if (this.queryDelayTimerId_)
        window.clearTimeout(this.queryDelayTimerId_);

      // Searching cookies uses a timeout of 500ms. We use a shorter timeout
      // because there are probably fewer passwords and we want the UI to be
      // snappier since users will expect that it's "less work."
      this.queryDelayTimerId_ = window.setTimeout(
          this.searchPasswords_.bind(this), 250);

      chrome.send('coreOptionsUserMetricsAction',
                  ['Options_ADPasswordManagerSearch']);
    },

    /**
     * Search passwords using text in |password-search-box|.
     * @private
     */
    searchPasswords_: function() {
      this.queryDelayTimerId_ = 0;
      var filter = $('ADpassword-search-box').value;
      filter = (filter == '') ? null : filter;
      if (this.lastQuery_ != filter) {
        this.lastQuery_ = filter;
        // Searching for passwords has the side effect of requerying the
        // underlying password store. This is done intentionally, as on OS X and
        // Linux they can change from outside and we won't be notified of it.
        chrome.send('updateADPasswordLists');
      }
    },

    /**
     * Updates the visibility of the list and empty list placeholder.
     * @param {!cr.ui.List} list The list to toggle visilibility for.
     */
    updateListVisibility_: function(list) {
      var empty = list.dataModel.length == 0;
      var listPlaceHolderID = list.id + '-empty-placeholder';
      list.hidden = empty;
      $(listPlaceHolderID).hidden = !empty;
    },

    /**
     * Updates the data model for the saved passwords list with the values from
     * |entries|.
     * @param {!Array} entries The list of saved password data.
     */
    setSavedADPasswordsList_: function(entries) {
      appList = entries;
      if (this.lastQuery_) {
        // Implement password searching here in javascript, rather than in C++.
        // The number of saved passwords shouldn't be too big for us to handle.
        var query = this.lastQuery_;
        var filter = function(entry, index, list) {
          // Search both shown URL and username.
          var shownUrl = entry[options.passwordADManager.SHOWN_URL_FIELD];
          var username = entry[options.passwordADManager.USERNAME_FIELD];
          var appName = entry[options.passwordADManager.APP_NAME_FIELD];
		  var iconUrl = entry[options.passwordADManager.ICON_URL_FIELD];
          if (shownUrl.toLowerCase().indexOf(query.toLowerCase()) >= 0 ||
              username.toLowerCase().indexOf(query.toLowerCase()) >= 0) {
            // Keep the original index so we can delete correctly. See also
            // deleteItemAtIndex() in password_manager_list.js that uses this.
            entry[options.passwordADManager.ORIGINAL_INDEX_FIELD] = index;
            return true;
          }
          return false;
        };
        entries = entries.filter(filter);
      }
      this.savedADPasswordsList_.dataModel = new ArrayDataModel(entries);
      this.updateListVisibility_(this.savedADPasswordsList_);
    },

    /**
     * Updates the data model for the password exceptions list with the values
     * from |entries|.
     * @param {!Array} entries The list of password exception data.
     */
    setPasswordExceptionsList_: function(entries) {
      this.passwordExceptionsList_.dataModel = new ArrayDataModel(entries);
      this.updateListVisibility_(this.passwordExceptionsList_);
    },

    /**
     * Reveals the password for a saved password entry. This is called by the
     * backend after it has authenticated the user.
     * @param {number} index The original index of the entry in the model.
     * @param {string} password The saved password.
     */
    showADPassword_: function(index, password) {
      var model = this.savedADPasswordsList_.dataModel;
      if (this.lastQuery_) {
        // When a filter is active, |index| does not represent the current
        // index in the model, but each entry stores its original index, so
        // we can find the item using a linear search.
        for (var i = 0; i < model.length; ++i) {
          if (model.item(i)[options.passwordADManager.ORIGINAL_INDEX_FIELD] ==
              index) {
            index = i;
            break;
          }
        }
      }

      // Reveal the password in the UI.
      var item = this.savedADPasswordsList_.getListItemByIndex(index);
      item.showPassword(password);
    },
  };

  /**
   * Removes a saved password.
   * @param {number} rowIndex indicating the row to remove.
   */
  PasswordADManager.removeSavedPassword = function(rowIndex) {
      chrome.send('removeSavedPassword', [String(rowIndex)]);
      chrome.send('coreOptionsUserMetricsAction',
                  ['Options_PasswordManagerDeletePassword']);
  };

  /**
   * Removes a password exception.
   * @param {number} rowIndex indicating the row to remove.
   */
  // PasswordADManager.removePasswordException = function(rowIndex) {
  //     chrome.send('removePasswordException', [String(rowIndex)]);
  // };

  PasswordADManager.requestShowADPassword = function(index) {
    chrome.send('requestShowADPassword', [index]);
  };

  // Forward public APIs to private implementations on the singleton instance.
  cr.makePublic(PasswordADManager, [
    'setSavedADPasswordsList',
    'showADPassword'
  ]);

  // Export
  return {
    PasswordADManager: PasswordADManager
  };

});

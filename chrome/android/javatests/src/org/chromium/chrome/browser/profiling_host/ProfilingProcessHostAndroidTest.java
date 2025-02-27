// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.profiling_host;

import android.support.test.filters.MediumTest;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import org.chromium.base.test.util.CommandLineFlags;
import org.chromium.chrome.browser.ChromeActivity;
import org.chromium.chrome.browser.ChromeSwitches;
import org.chromium.chrome.test.ChromeActivityTestRule;
import org.chromium.chrome.test.ChromeJUnit4ClassRunner;

/**
 * Test suite for out of process heap profiling.
 */
@RunWith(ChromeJUnit4ClassRunner.class)
@CommandLineFlags.Add({ChromeSwitches.DISABLE_FIRST_RUN_EXPERIENCE})
public class ProfilingProcessHostAndroidTest {
    private static final String TAG = "ProfilingProcessHostAndroidTest";
    @Rule
    public ChromeActivityTestRule<ChromeActivity> mActivityTestRule =
            new ChromeActivityTestRule<>(ChromeActivity.class);

    @Before
    public void setUp() throws InterruptedException {
        mActivityTestRule.startMainActivityOnBlankPage();
    }

    @Test
    @MediumTest
    @CommandLineFlags.Add({"memlog=browser"})
    public void testModeBrowser() throws Exception {
        TestAndroidShim profilingProcessHost = new TestAndroidShim();
        Assert.assertTrue(profilingProcessHost.runTestForMode("browser", false, false));
    }

    @Test
    @MediumTest
    public void testModeBrowserDynamic() throws Exception {
        TestAndroidShim profilingProcessHost = new TestAndroidShim();
        Assert.assertTrue(profilingProcessHost.runTestForMode("browser", true, false));
    }

    @Test
    @MediumTest
    public void testModeBrowserDynamicPseudo() throws Exception {
        TestAndroidShim profilingProcessHost = new TestAndroidShim();
        Assert.assertTrue(profilingProcessHost.runTestForMode("browser", true, true));
    }

    @Test
    @MediumTest
    @CommandLineFlags.Add({"memlog=all-renderers", "memlog-stack-mode=pseudo"})
    public void testModeRendererPseudo() throws Exception {
        TestAndroidShim profilingProcessHost = new TestAndroidShim();
        Assert.assertTrue(profilingProcessHost.runTestForMode("all-renderers", false, true));
    }

    @Test
    @MediumTest
    public void testModeGpuDynamicPseudo() throws Exception {
        TestAndroidShim profilingProcessHost = new TestAndroidShim();
        Assert.assertTrue(profilingProcessHost.runTestForMode("gpu", true, true));
    }
}

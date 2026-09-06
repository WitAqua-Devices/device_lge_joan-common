/*
 * SPDX-FileCopyrightText: 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package com.lge.systemservice.core.halinterface;

/**
 * The WLAN factory-test interface, reached through
 * LGContext.getLGSystemService("wifiLgeHalManagerService"). Every one of these
 * drives the chip's RF test mode through LG's own HAL, which is not part of
 * this build - so the FTM screens come up and report that the chip refused,
 * rather than taking the whole hidden menu down with them.
 */
public class WifiLgeHalManager {

    public boolean OpenDUT_HiddenMenu(boolean on) {
        return false;
    }

    public boolean CloseDUT_HiddenMenu(boolean on) {
        return false;
    }

    public boolean Channel_HiddenMenu(int channel, int band) {
        return false;
    }

    public boolean Channel5G_HiddenMenu(int channel, int band) {
        return false;
    }

    public int CodeRate_HiddenMenu(String rate) {
        return -1;
    }

    public boolean Create_Rsdb_iface_HiddenMenu() {
        return false;
    }

    public int FRError_HiddenMenu() {
        return -1;
    }

    public int FRGood_HiddenMenu() {
        return -1;
    }

    public boolean NoModTxStart_BCM_HiddenMenu(int mode) {
        return false;
    }

    public boolean NoModTxStart_HiddenMenu() {
        return false;
    }

    public boolean NoModTxStop_HiddenMenu() {
        return false;
    }

    public int OpMode_HiddenMenu(String mode) {
        return -1;
    }

    public boolean RxDataRate11ac_HiddenMenu(int rate) {
        return false;
    }

    public int RxPER_HiddenMenu(String param) {
        return -1;
    }

    public boolean RxStart_HiddenMenu(boolean on) {
        return false;
    }

    public boolean RxStop_HiddenMenu(boolean on) {
        return false;
    }

    public boolean Set11nPreamble_HiddenMenu(int preamble) {
        return false;
    }

    public boolean SetChain_HiddenMenu(int chain) {
        return false;
    }

    public boolean SetPreamble_HiddenMenu(int preamble) {
        return false;
    }

    public boolean SetRSDB_HiddenMenu(int mode) {
        return false;
    }

    public boolean SetRegDomain_HiddenMenu(int domain) {
        return false;
    }

    public boolean SetRftInterface_HiddenMenu(int iface) {
        return false;
    }

    public boolean SetSarMode_HiddenMenu(int mode) {
        return false;
    }

    public boolean SetTxBF_HiddenMenu(int mode) {
        return false;
    }

    public boolean TXBW_40M_HiddenMenu(int bw) {
        return false;
    }

    public boolean TXBW_80M_HiddenMenu(int bw) {
        return false;
    }

    public boolean TxBurstFrames_HiddenMenu(int frames) {
        return false;
    }

    public boolean TxBurstInterval_HiddenMenu(int interval) {
        return false;
    }

    public boolean TxDataRate11ac_HiddenMenu(int a, int b, int c, int d, int e) {
        return false;
    }

    public boolean TxDataRate11n40M_HiddenMenu(int a, int b, int c) {
        return false;
    }

    public boolean TxDataRate11n5G40M_HiddenMenu(int a, int b, int c) {
        return false;
    }

    public boolean TxDataRate11n5G_HiddenMenu(int a, int b, int c) {
        return false;
    }

    public boolean TxDataRate11n_HiddenMenu(int a, int b, int c) {
        return false;
    }

    public boolean TxDataRate5G_HiddenMenu(String rate) {
        return false;
    }

    public boolean TxDataRate_HiddenMenu(String rate) {
        return false;
    }

    public boolean TxDestAddress_HiddenMenu(String address) {
        return false;
    }

    public boolean TxGain_HiddenMenu(int gain) {
        return false;
    }

    public int TxPER_HiddenMenu(String param) {
        return -1;
    }

    public boolean TxPayloadLength_HiddenMenu(int length) {
        return false;
    }

    public boolean TxStart_HiddenMenu(boolean on) {
        return false;
    }

    public boolean TxStop_HiddenMenu(boolean on) {
        return false;
    }
}

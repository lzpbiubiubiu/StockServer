// Copyright(c) 2021 Dmall POS Project

pragma Singleton
import QtQml 2.12
import QtQuick 2.12
import QtQuick.Window 2.12

QtObject
{
    id: config
    objectName: "config"

    // 全局字体
    property string fontFamily: "Plus Jakarta Sans" //__winxp ? "SimSun" : "Microsoft YaHei UI"
}

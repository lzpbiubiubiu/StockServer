// Copyright (c) 2021 Dmall Pos Team

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QList>
#include "Extras/TerminalManager.h"

namespace UI
{
    /** 设备列表数据 */
    class TerminalPanelModel : public QAbstractListModel
    {
        Q_OBJECT
    public:
        /** 设备列表项 */
        struct TerminalItem
        {
            /** ID */
            QString id;

            /** 银台号 */
            QString deskNo;

            /** 设备编码 */
            QString deviceCode;

            /** 未上传离线 */
            QString unsyncTask;

            /** 是否连接 */
            bool connected = false;
        };

        enum ItemRole
        {
            /** ID */
            ID_ROLE = Qt::UserRole + 1,

            /** 银台号 */
            DESKNO_ROLE,

            /** 设备编码 */
            DEVICECODE_ROLE,

            /** 离线任务 */
            UNSYNC_TASK,

            /** 是否连接 */
            CONNECTED_ROLE
        };

        TerminalPanelModel(QObject* parent = nullptr);
        ~TerminalPanelModel();

        /** 设置列表数据 */
        void SetItems(const QList<TerminalItem>& items);

        /** 更新列表项 */
        void UpdateItem(const TerminalItem& item);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        virtual QVariant data(const QModelIndex& index, int role) const override;

        virtual QHash<int, QByteArray> roleNames() const override;

    private:
        /** 设备列表 */
        QList<TerminalItem> m_items;
    };

    /** 设备列表面板 */
    class TerminalPanel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QObject* posModel MEMBER m_posModelData CONSTANT)
        Q_PROPERTY(QObject* pvtModel MEMBER m_pvtModelData CONSTANT)

    public:      
        TerminalPanel(QObject* parent);
        ~TerminalPanel();

        TerminalPanelModel::TerminalItem GetPanelItem(const Extra::TerminalManager::Terminal& terminal) const;

        void InitPos(const QList<Extra::TerminalManager::Terminal>& posTerminals);
        void InitPvt(const QList<Extra::TerminalManager::Terminal>& pvtTerminals);

        bool UpdatePosTerminalItem(const Extra::TerminalManager::Terminal& terminal);

        bool UpdatePvtTerminalItem(const Extra::TerminalManager::Terminal& terminal);
    Q_SIGNALS:
    private:
        // pos设备列表数据
        TerminalPanelModel* m_posModelData = nullptr;

        // pvt设备列表数据
        TerminalPanelModel* m_pvtModelData = nullptr;
    };
}
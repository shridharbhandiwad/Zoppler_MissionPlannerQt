#include "radarlistpanel.h"
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QFont>

static const QString kPanelStyle = R"(
QWidget#RadarListPanel {
    background-color: #1e1e2a;
    border-right: 1px solid #2e3a4a;
}
QLabel#titleLabel {
    color: #00ff88;
    font-size: 13px;
    font-weight: bold;
    padding: 8px 12px;
    background-color: #12121c;
    border-bottom: 1px solid #2e3a4a;
}
QListWidget {
    background-color: #1a1a28;
    border: none;
    color: #d0d8e8;
    font-size: 11px;
    outline: none;
}
QListWidget::item {
    padding: 10px 12px;
    border-bottom: 1px solid #2a2a3a;
}
QListWidget::item:selected {
    background-color: #1a4a3a;
    color: #00ff88;
    border-left: 3px solid #00ff88;
}
QListWidget::item:hover {
    background-color: #252538;
}
QPushButton#openButton {
    background-color: #1a4a3a;
    color: #00ff88;
    border: 1px solid #00aa55;
    border-radius: 4px;
    padding: 8px 16px;
    font-weight: bold;
    font-size: 11px;
    margin: 8px;
}
QPushButton#openButton:hover {
    background-color: #206040;
    border-color: #00ff88;
}
QPushButton#openButton:pressed {
    background-color: #0a3020;
}
)";

RadarListPanel::RadarListPanel(RadarManager *manager, QWidget *parent)
    : QWidget(parent)
    , m_manager(manager)
{
    setObjectName("RadarListPanel");
    setStyleSheet(kPanelStyle);
    setMinimumWidth(240);
    setMaximumWidth(320);
    buildUI();
    populateList();

    connect(m_manager, &RadarManager::radarDataChanged, this, &RadarListPanel::refresh);
}

void RadarListPanel::buildUI()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *titleLabel = new QLabel("RADAR LIST", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(titleLabel);

    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    root->addWidget(m_listWidget, 1);

    m_openButton = new QPushButton("Open PPI View", this);
    m_openButton->setObjectName("openButton");
    root->addWidget(m_openButton);

    connect(m_listWidget, &QListWidget::itemDoubleClicked,
            this, &RadarListPanel::onItemDoubleClicked);
    connect(m_openButton, &QPushButton::clicked,
            this, &RadarListPanel::onOpenClicked);
}

void RadarListPanel::populateList()
{
    m_listWidget->clear();

    for (const auto &radar : m_manager->radars()) {
        QString statusStr = radar.active ? "Active" : "Inactive";
        QString label = QString("[%1]  %2\n"
                                "ID: %3   Status: %4")
                            .arg(radar.radarId)
                            .arg(radar.radarName)
                            .arg(radar.radarId)
                            .arg(statusStr);

        auto *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, radar.radarId);

        // Colour-code by status
        if (radar.active) {
            item->setForeground(QColor("#00ff88"));
        } else {
            item->setForeground(QColor("#888899"));
            QFont f = item->font();
            f.setItalic(true);
            item->setFont(f);
        }

        m_listWidget->addItem(item);
    }
}

void RadarListPanel::refresh()
{
    populateList();
}

void RadarListPanel::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;
    int id = item->data(Qt::UserRole).toInt();
    emit radarSelected(id);
}

void RadarListPanel::onOpenClicked()
{
    auto *item = m_listWidget->currentItem();
    if (!item) return;
    int id = item->data(Qt::UserRole).toInt();
    emit radarSelected(id);
}

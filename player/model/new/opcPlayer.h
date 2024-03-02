#pragma once

#include <QtCore/QObject>

#include "player/model/new/Player.h"
//#include "OPC_UA.h"

class QTimer;

class opcPlayer : public Player
{
	Q_OBJECT

public:

	explicit opcPlayer(Player* parent = nullptr);
	~opcPlayer() override = default;

	QString sourceName() const override;

	void setDataSource(const QString &serverName) override;

signals:
	void fragmentRead(SignalFragment& fragment);
	void reset();
	void error(const QString& message);

public slots:
	void readNext() override;
	void stop() override;

	//void onFileOpenError();
	//void onFileParseError();

private:
	//std::unique_ptr<OPC_UAr> m_opcReader;
};

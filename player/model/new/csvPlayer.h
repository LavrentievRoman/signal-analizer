#pragma once

#include <memory>

#include <QtCore/QObject>

#include "player/model/new/Player.h"
#include "player/model/csv-signal-fragment-reader.h"

class QString;
class QTimer;

class csvPlayer : public Player
{
	Q_OBJECT

public:
	explicit csvPlayer(Player* parent = nullptr);
	~csvPlayer() override = default;

	QString sourceName() const override;

	void setDataSource(const QString& fileName) override;

signals:
    void fragmentRead(SignalFragment& fragment);
    void reset();
    void error(const QString& message);

public slots:
	void readNext() override;
	void stop() override;

	void onFileOpenError();
	void onFileParseError();

private:
	std::unique_ptr<CsvSignalFragmentReader> m_csvReader;

};

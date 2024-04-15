#ifndef FILECOMBOBOX_H
#define FILECOMBOBOX_H

#include <QtWidgets>


class FileComboBox : public QComboBox {
    Q_OBJECT
public:
    FileComboBox(QWidget *parent = nullptr) : QComboBox(parent) {
        // Добавляем первый элемент, который позволит выбрать папку
        addItem("Выбрать папку...");

        connect(this, QOverload<int>::of(&QComboBox::activated), this, &FileComboBox::onActivated);
    }

private slots:
    void onActivated(int index) {
        if (index == 0) {
            QString directory = QFileDialog::getExistingDirectory(this, "Выбрать папку", QDir::homePath());
            if (!directory.isEmpty()) {
                clear();
                addItem("Выбрать папку...");
                QDir dir(directory);
                QFileInfoList files = dir.entryInfoList(QDir::Files);
                foreach (const QFileInfo& file, files) {
                    addItem(file.fileName(), file.absoluteFilePath());
                }
                QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
                foreach (const QFileInfo& d, dirs) {
                    addItem(d.fileName(), d.absoluteFilePath());
                }

            }
        } else {
            QString filePath = itemData(index).toString();
            emit fileSelected(filePath);
        }
    }

signals:
    void fileSelected(const QString &filePath);
};

#endif // FILECOMBOBOX_H

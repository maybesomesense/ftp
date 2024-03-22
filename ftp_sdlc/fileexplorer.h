#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H
#include <QComboBox>
#include <QDir>

class fileexplorer : public QObject
{
    Q_OBJECT
public:
    fileexplorer(QComboBox *comboBox) : comboBox(comboBox) {
        // При старте приложения показываем содержимое текущей директории
        //QDir directory("D:/"); // Устанавливаем стартовую папку на диске D:
        updateComboBox(QDir::currentPath());

        //updateComboBox(QDir::toNativeSeparators("D:\\"));

        // Соединяем событие выбора в comboBox с обработчиком
        connect(comboBox, QOverload<int>::of(&QComboBox::activated),
                [=](int index){ handleComboBoxActivated(index); });
    }


private slots:
    void handleComboBoxActivated(int index) {
        // Получаем путь к папке, выбранной в comboBox
        QString selectedFolder = comboBox->itemText(index);

        // Обновляем содержимое comboBox с учетом выбранной папки
        updateComboBox(selectedFolder);
    }

private:
    void updateComboBox(const QString &path) {
        QDir directory(path);
        QStringList folders = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        QStringList files = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

        comboBox->clear();
        comboBox->addItem(".."); // Добавляем пункт для перехода на уровень выше

        foreach(QString folder, folders) {
            comboBox->addItem(folder); // Добавляем папки в comboBox
        }

        foreach(QString file, files) {
            comboBox->addItem(file); // Добавляем файлы в comboBox
        }
    }

    QComboBox *comboBox;
};

#endif // FILEEXPLORER_H

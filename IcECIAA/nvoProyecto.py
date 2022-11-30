# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'nvoProyecto.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_nvoProyecto(object):
    def setupUi(self, nvoProyecto):
        nvoProyecto.setObjectName("nvoProyecto")
        nvoProyecto.resize(630, 658)
        font = QtGui.QFont()
        font.setStyleStrategy(QtGui.QFont.PreferDefault)
        nvoProyecto.setFont(font)
        self.textEdit_2 = QtWidgets.QTextEdit(nvoProyecto)
        self.textEdit_2.setGeometry(QtCore.QRect(10, 330, 611, 101))
        self.textEdit_2.setObjectName("textEdit_2")
        self.label_4 = QtWidgets.QLabel(nvoProyecto)
        self.label_4.setGeometry(QtCore.QRect(10, 460, 261, 31))
        self.label_4.setObjectName("label_4")
        self.label_3 = QtWidgets.QLabel(nvoProyecto)
        self.label_3.setGeometry(QtCore.QRect(10, 300, 261, 31))
        self.label_3.setObjectName("label_3")
        self.textEdit_3 = QtWidgets.QTextEdit(nvoProyecto)
        self.textEdit_3.setGeometry(QtCore.QRect(10, 490, 611, 101))
        self.textEdit_3.setObjectName("textEdit_3")
        self.textEdit = QtWidgets.QTextEdit(nvoProyecto)
        self.textEdit.setGeometry(QtCore.QRect(10, 170, 611, 101))
        self.textEdit.setObjectName("textEdit")
        self.label = QtWidgets.QLabel(nvoProyecto)
        self.label.setGeometry(QtCore.QRect(10, 20, 141, 20))
        self.label.setObjectName("label")
        self.lineEdit = QtWidgets.QLineEdit(nvoProyecto)
        self.lineEdit.setGeometry(QtCore.QRect(170, 20, 331, 30))
        self.lineEdit.setObjectName("lineEdit")
        self.label_2 = QtWidgets.QLabel(nvoProyecto)
        self.label_2.setGeometry(QtCore.QRect(10, 140, 191, 20))
        self.label_2.setObjectName("label_2")
        self.pushButton = QtWidgets.QPushButton(nvoProyecto)
        self.pushButton.setGeometry(QtCore.QRect(530, 620, 90, 28))
        self.pushButton.setObjectName("pushButton")
        self.pushButton_2 = QtWidgets.QPushButton(nvoProyecto)
        self.pushButton_2.setGeometry(QtCore.QRect(420, 620, 90, 28))
        self.pushButton_2.setObjectName("pushButton_2")
        self.lineEdit_2 = QtWidgets.QLineEdit(nvoProyecto)
        self.lineEdit_2.setGeometry(QtCore.QRect(70, 80, 171, 30))
        self.lineEdit_2.setText("")
        self.lineEdit_2.setObjectName("lineEdit_2")
        self.label_5 = QtWidgets.QLabel(nvoProyecto)
        self.label_5.setGeometry(QtCore.QRect(10, 80, 51, 20))
        self.label_5.setObjectName("label_5")

        self.retranslateUi(nvoProyecto)
        QtCore.QMetaObject.connectSlotsByName(nvoProyecto)

    def retranslateUi(self, nvoProyecto):
        _translate = QtCore.QCoreApplication.translate
        nvoProyecto.setWindowTitle(_translate("nvoProyecto", "Nuevo Proyecto"))
        self.label_4.setText(_translate("nvoProyecto", "Encabezado Archivo Principal M4"))
        self.label_3.setText(_translate("nvoProyecto", "Encabezado Archivo Principal M0"))
        self.label.setText(_translate("nvoProyecto", "Nombre del Proyecto"))
        self.label_2.setText(_translate("nvoProyecto", "Descripción del Proyecto"))
        self.pushButton.setText(_translate("nvoProyecto", "Cancelar"))
        self.pushButton_2.setText(_translate("nvoProyecto", "Aceptar"))
        self.label_5.setText(_translate("nvoProyecto", "Autor"))


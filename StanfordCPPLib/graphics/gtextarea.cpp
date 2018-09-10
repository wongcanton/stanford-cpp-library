/*
 * File: gtextarea.cpp
 * -------------------
 *
 * @author Marty Stepp
 * @version 2018/08/23
 * - renamed to gtextarea.cpp to replace Java version
 * @version 2018/06/25
 * - initial version
 */

#include "gtextarea.h"
#include <QScrollBar>
#include <QTextCursor>
#include "gcolor.h"
#include "gfont.h"
#include "gthread.h"
#include "gwindow.h"
#include "require.h"
#include "strlib.h"

GTextArea::GTextArea(int rows, int columns, QWidget* parent)
        : _contextMenuEnabled(true) {
    GThread::runOnQtGuiThread([this, parent]() {
        _iqtextedit = new _Internal_QTextEdit(this, getInternalParent(parent));
    });
    setRowsColumns(rows, columns);
    setVisible(false);   // all widgets are not shown until added to a window
}

GTextArea::GTextArea(const std::string& text, QWidget* parent)
        : _contextMenuEnabled(true) {
    GThread::runOnQtGuiThread([this, parent]() {
        _iqtextedit = new _Internal_QTextEdit(this, getInternalParent(parent));
    });
    setText(text);
    setVisible(false);   // all widgets are not shown until added to a window
}

GTextArea::~GTextArea() {
    // TODO: delete _iqtextedit;
    _iqtextedit = nullptr;
}

void GTextArea::appendFormattedText(const std::string& text, const std::string& color, const std::string& font) {
    moveCursorToEnd();

    // create a formatted block with the font and color
    QTextCharFormat format;
    if (!color.empty()) {
        format.setForeground(QBrush(GColor::convertColorToRGB(color)));
    }
    if (!font.empty()) {
        format.setFont(GFont::toQFont(_iqtextedit->font(), font));
    }

    QTextCursor cursor = _iqtextedit->textCursor();
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    cursor.insertText(QString::fromStdString(text), format);
    cursor.endEditBlock();
    GThread::runOnQtGuiThread([this, cursor]() {
        _iqtextedit->setTextCursor(cursor);
        _iqtextedit->ensureCursorVisible();
    });

    moveCursorToEnd();
}

void GTextArea::appendHtml(const std::string& html) {
    // TODO: use insertHtml for speed?
    setHtml(getHtml() + html);
}

void GTextArea::appendText(const std::string& text) {
    QTextCursor cursor = _iqtextedit->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
    cursor.insertText(QString::fromStdString(text));
    moveCursorToEnd();
}

void GTextArea::clearSelection() {
    GThread::runOnQtGuiThread([this]() {
        QTextCursor cursor = _iqtextedit->textCursor();
        cursor.clearSelection();
        _iqtextedit->setTextCursor(cursor);
    });
}

void GTextArea::clearText() {
    GThread::runOnQtGuiThread([this]() {
        _iqtextedit->clear();
    });
}

int GTextArea::getColumns() const {
    return (int) (getHeight() / getRowColumnSize().getWidth());
}

int GTextArea::getCursorPosition() const {
    return _iqtextedit->textCursor().position();
}

std::string GTextArea::getHtml() const {
    return _iqtextedit->toHtml().toStdString();
}

_Internal_QWidget* GTextArea::getInternalWidget() const {
    return _iqtextedit;
}

std::string GTextArea::getPlaceholder() const {
    return _iqtextedit->placeholderText().toStdString();
}

GDimension GTextArea::getRowColumnSize() const {
    QFontMetrics metrics(_iqtextedit->font());
    return GDimension(metrics.width(QString::fromStdString("mmmmmmmmmm")) / 10.0, metrics.lineSpacing() + 2);
}

int GTextArea::getRows() const {
    return (int) (getHeight() / getRowColumnSize().getHeight());
}

std::string GTextArea::getSelectedText() const {
    QTextCursor cursor = _iqtextedit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if (end > start) {
        return getText().substr(start, end - start);
    } else {
        return "";
    }
}

int GTextArea::getSelectionEnd() const {
    QTextCursor cursor = _iqtextedit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if (end > start) {
        return end;
    } else {
        // no selection; cursor sets selection start/end to be equal
        return -1;
    }
}

int GTextArea::getSelectionLength() const {
    QTextCursor cursor = _iqtextedit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    return end - start;
}

int GTextArea::getSelectionStart() const {
    QTextCursor cursor = _iqtextedit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if (end > start) {
        return start;
    } else {
        return -1;
    }
}

std::string GTextArea::getText() const {
    return _iqtextedit->toPlainText().toStdString();
}

std::string GTextArea::getType() const {
    return "GTextArea";
}

QWidget* GTextArea::getWidget() const {
    return static_cast<QWidget*>(_iqtextedit);
}

bool GTextArea::isContextMenuEnabled() const {
    return _contextMenuEnabled;
}

bool GTextArea::isEditable() const {
    return !_iqtextedit->isReadOnly();
}

bool GTextArea::isLineWrap() const {
    return _iqtextedit->lineWrapMode() != QTextEdit::NoWrap;
}

void GTextArea::moveCursorToEnd() {
    GThread::runOnQtGuiThread([this]() {
        QTextCursor cursor = _iqtextedit->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor, 1);
        _iqtextedit->setTextCursor(cursor);
        _iqtextedit->ensureCursorVisible();
    });
}

void GTextArea::moveCursorToStart() {
    GThread::runOnQtGuiThread([this]() {
        QTextCursor cursor = _iqtextedit->textCursor();
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
        _iqtextedit->setTextCursor(cursor);
        _iqtextedit->ensureCursorVisible();
    });
}

void GTextArea::removeKeyListener() {
    removeEventListeners({"keypress",
                          "keyrelease",
                          "keytype"});
}

void GTextArea::removeMouseListener() {
    removeEventListeners({"mousepress",
                          "mouserelease"});
}

void GTextArea::removeTextChangeListener() {
    removeEventListener("textchange");
}

void GTextArea::scrollToBottom() {
    GThread::runOnQtGuiThread([this]() {
        QScrollBar* scrollbar = _iqtextedit->verticalScrollBar();
        scrollbar->setValue(scrollbar->maximum());
        scrollbar->setSliderPosition(scrollbar->maximum());
    });
}

void GTextArea::scrollToTop() {
    GThread::runOnQtGuiThread([this]() {
        QScrollBar* scrollbar = _iqtextedit->verticalScrollBar();
        scrollbar->setValue(0);
        scrollbar->setSliderPosition(0);
    });
}

void GTextArea::select(int startIndex, int length) {
    require::nonNegative(startIndex, 0, "GTextArea::select", "startIndex");
    require::nonNegative(length, 0, "GTextArea::select", "length");
    GThread::runOnQtGuiThread([this, startIndex, length]() {
        QTextCursor cursor = _iqtextedit->textCursor();
        cursor.setPosition(startIndex);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, length);
        _iqtextedit->setTextCursor(cursor);
    });
}

void GTextArea::selectAll() {
    GThread::runOnQtGuiThread([this]() {
        _iqtextedit->selectAll();
    });
}

void GTextArea::setColumns(int columns) {
    require::nonNegative(columns, "GTextArea::setColumns");
    double desiredWidth = getRowColumnSize().getWidth() * columns;
    setPreferredSize(desiredWidth, getHeight());
    setSize(desiredWidth, getHeight());
}

void GTextArea::setContextMenuEnabled(bool enabled) {
    _contextMenuEnabled = enabled;
}

void GTextArea::setCursorPosition(int index, bool keepAnchor) {
    require::nonNegative(index, "TextArea::setCursorPosition", "index");
    GThread::runOnQtGuiThread([this, index, keepAnchor]() {
        QTextCursor cursor(_iqtextedit->textCursor());
        cursor.setPosition(index, keepAnchor ? QTextCursor::KeepAnchor : QTextCursor::MoveAnchor);
        _iqtextedit->setTextCursor(cursor);
        _iqtextedit->ensureCursorVisible();
    });
}

void GTextArea::setEditable(bool value) {
    GThread::runOnQtGuiThread([this, value]() {
        _iqtextedit->setReadOnly(!value);
    });
}

void GTextArea::setHtml(const std::string& html) {
    GThread::runOnQtGuiThread([this, html]() {
        _iqtextedit->setHtml(QString::fromStdString(html));
    });
}

void GTextArea::setPlaceholder(const std::string& text) {
    GThread::runOnQtGuiThread([this, text]() {
        _iqtextedit->setPlaceholderText(QString::fromStdString(text));
    });
}

void GTextArea::setRows(int rows) {
    require::nonNegative(rows, "GTextArea::setRows");
    double desiredHeight = getRowColumnSize().getHeight() * rows;
    setPreferredSize(getWidth(), desiredHeight);
    setSize(getWidth(), desiredHeight);
}

void GTextArea::setRowsColumns(int rows, int columns) {
    require::nonNegative2D(rows, columns, "GTextArea::setRowsColumns", "rows", "columns");
    double desiredWidth = getRowColumnSize().getWidth() * columns;
    double desiredHeight = getRowColumnSize().getHeight() * rows;
    setPreferredSize(desiredWidth, desiredHeight);
    setSize(desiredWidth, desiredHeight);
}

void GTextArea::setText(const std::string& text) {
    GThread::runOnQtGuiThread([this, text]() {
        _iqtextedit->setText(QString::fromStdString(text));
    });
}

void GTextArea::setKeyListener(GEventListener func) {
    GThread::runOnQtGuiThread([this]() {
        _iqtextedit->setFocusPolicy(Qt::StrongFocus);
    });
    setEventListeners({"keypress",
                       "keyrelease",
                       "keytype"}, func);
}

void GTextArea::setKeyListener(GEventListenerVoid func) {
    GThread::runOnQtGuiThread([this]() {
        _iqtextedit->setFocusPolicy(Qt::StrongFocus);
    });
    setEventListeners({"keypress",
                       "keyrelease",
                       "keytype"}, func);
}

void GTextArea::setMouseListener(GEventListener func) {
    setEventListeners({"mousepress",
                       "mouserelease"}, func);
}

void GTextArea::setMouseListener(GEventListenerVoid func) {
    setEventListeners({"mousepress",
                       "mouserelease"}, func);
}

void GTextArea::setLineWrap(bool wrap) {
    GThread::runOnQtGuiThread([this, wrap]() {
        _iqtextedit->setLineWrapMode(wrap ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    });
}

void GTextArea::setTextChangeListener(GEventListener func) {
    setEventListener("textchange", func);
}

void GTextArea::setTextChangeListener(GEventListenerVoid func) {
    setEventListener("textchange", func);
}


_Internal_QTextEdit::_Internal_QTextEdit(GTextArea* gtextArea, QWidget* parent)
        : QTextEdit(parent),
          _gtextarea(gtextArea) {
    require::nonNull(gtextArea, "_Internal_QTextEdit::constructor");
    setObjectName(QString::fromStdString("_Internal_QTextEdit_" + integerToString(gtextArea->getID())));
    ensureCursorVisible();
    this->document()->setUndoRedoEnabled(false);
    connect(this, SIGNAL(textChanged()), this, SLOT(handleTextChange()));
}

void _Internal_QTextEdit::contextMenuEvent(QContextMenuEvent* event) {
    if (_gtextarea->isContextMenuEnabled()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void _Internal_QTextEdit::handleTextChange() {
    GEvent textChangeEvent(
                /* class  */ KEY_EVENT,
                /* type   */ KEY_TYPED,
                /* name   */ "textchange",
                /* source */ _gtextarea);
    textChangeEvent.setActionCommand(_gtextarea->getActionCommand());
    _gtextarea->fireEvent(textChangeEvent);
}

void _Internal_QTextEdit::keyPressEvent(QKeyEvent* event) {
    require::nonNull(event, "_Internal_QTextEdit::keyPressEvent", "event");
    event->accept();
    _gtextarea->fireGEvent(event, KEY_PRESSED, "keypress");
    if (event->isAccepted()) {
        QTextEdit::keyPressEvent(event);   // call super
    }
}

void _Internal_QTextEdit::keyReleaseEvent(QKeyEvent* event) {
    require::nonNull(event, "_Internal_QTextEdit::keyPressEvent", "event");
    event->accept();
    _gtextarea->fireGEvent(event, KEY_RELEASED, "keyrelease");
    if (event->isAccepted()) {
        QTextEdit::keyReleaseEvent(event);   // call super
    }
}

void _Internal_QTextEdit::mousePressEvent(QMouseEvent* event) {
    require::nonNull(event, "_Internal_QTextEdit::keyPressEvent", "event");
    event->accept();
    if (!_gtextarea->isAcceptingEvent("mousepress")) return;
    if (_gtextarea->isAcceptingEvent("mousepress")) {
        _gtextarea->fireGEvent(event, MOUSE_PRESSED, "mousepress");
    }
    if (event->isAccepted()) {
        QTextEdit::mousePressEvent(event);   // call super
    }
}

void _Internal_QTextEdit::mouseReleaseEvent(QMouseEvent* event) {
    require::nonNull(event, "_Internal_QTextEdit::keyPressEvent", "event");
    event->accept();
    if (!_gtextarea->isAcceptingEvent("mouserelease")) return;
    if (_gtextarea->isAcceptingEvent("mouserelease")) {
        _gtextarea->fireGEvent(event, MOUSE_RELEASED, "mouserelease");
    }
    if (event->isAccepted()) {
        QTextEdit::mouseReleaseEvent(event);   // call super
    }
}

QSize _Internal_QTextEdit::sizeHint() const {
    if (hasPreferredSize()) {
        return getPreferredSize();
    } else {
        return QTextEdit::sizeHint();
    }
}

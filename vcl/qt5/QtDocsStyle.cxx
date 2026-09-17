/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QtDocsStyle.hxx>

#include <cstdio>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include <QtWidgets/QStyleOption>

namespace
{
// A restrained, light palette. Kept in one place so the painting code below
// and the palette handed to QApplication cannot drift apart.
const QColor aSurface(0xff, 0xff, 0xff);
const QColor aSurfaceMuted(0xf1, 0xf3, 0xf4);
const QColor aSurfaceSunken(0xe8, 0xea, 0xed);
const QColor aOutline(0xda, 0xdc, 0xe0);
const QColor aAccent(0x1a, 0x73, 0xe8);
const QColor aAccentMuted(0xe8, 0xf0, 0xfe);
const QColor aText(0x20, 0x21, 0x24);
const QColor aTextMuted(0x5f, 0x63, 0x68);
const QColor aTextDisabled(0xbd, 0xc1, 0xc6);

/// Corner radius for anything that reads as a button.
constexpr qreal fRadius = 6.0;

/// Fill used for a control in its current interaction state.
QColor stateFill(const QStyleOption* pOption, const QColor& rResting)
{
    if (!(pOption->state & QStyle::State_Enabled))
        return aSurfaceMuted;
    if (pOption->state & QStyle::State_Sunken)
        return aSurfaceSunken;
    if (pOption->state & QStyle::State_MouseOver)
        return aSurfaceMuted;
    return rResting;
}

void fillRounded(QPainter* pPainter, const QRect& rRect, const QColor& rFill,
                 const QColor& rBorder = QColor())
{
    pPainter->save();
    pPainter->setRenderHint(QPainter::Antialiasing, true);
    pPainter->setBrush(rFill);
    pPainter->setPen(rBorder.isValid() ? QPen(rBorder, 1) : QPen(Qt::NoPen));
    // Half-pixel inset so the 1px border lands on the pixel grid rather than
    // straddling it, which would render as a blurry two-pixel line.
    const QRectF aRect = rBorder.isValid() ? QRectF(rRect).adjusted(0.5, 0.5, -0.5, -0.5)
                                           : QRectF(rRect);
    pPainter->drawRoundedRect(aRect, fRadius, fRadius);
    pPainter->restore();
}
}

QtDocsStyle::QtDocsStyle()
    // Fusion computes every metric itself instead of asking a desktop theme,
    // so it is the only base that behaves the same in a browser as on a desktop.
    : QProxyStyle(QStyleFactory::create(QStringLiteral("fusion")))
{
}

QPalette QtDocsStyle::palette()
{
    QPalette aPalette;
    aPalette.setColor(QPalette::Window, aSurface);
    aPalette.setColor(QPalette::WindowText, aText);
    aPalette.setColor(QPalette::Base, aSurface);
    aPalette.setColor(QPalette::AlternateBase, aSurfaceMuted);
    aPalette.setColor(QPalette::Text, aText);
    aPalette.setColor(QPalette::Button, aSurface);
    aPalette.setColor(QPalette::ButtonText, aText);
    aPalette.setColor(QPalette::Highlight, aAccent);
    aPalette.setColor(QPalette::HighlightedText, aSurface);
    aPalette.setColor(QPalette::ToolTipBase, aText);
    aPalette.setColor(QPalette::ToolTipText, aSurface);
    aPalette.setColor(QPalette::Light, aSurface);
    aPalette.setColor(QPalette::Midlight, aSurfaceMuted);
    aPalette.setColor(QPalette::Mid, aOutline);
    aPalette.setColor(QPalette::Dark, aOutline);
    aPalette.setColor(QPalette::Shadow, aOutline);

    aPalette.setColor(QPalette::Disabled, QPalette::Text, aTextDisabled);
    aPalette.setColor(QPalette::Disabled, QPalette::ButtonText, aTextDisabled);
    aPalette.setColor(QPalette::Disabled, QPalette::WindowText, aTextDisabled);
    return aPalette;
}

int QtDocsStyle::pixelMetric(PixelMetric metric, const QStyleOption* option,
                             const QWidget* widget) const
{
    switch (metric)
    {
        // Flat: no sunken/raised bevels anywhere.
        case PM_DefaultFrameWidth:
        case PM_MenuPanelWidth:
        case PM_ToolBarFrameWidth:
        case PM_DockWidgetFrameWidth:
            return 0;
        // Toolbars read as a row of separate targets rather than a solid bar.
        case PM_ToolBarItemSpacing:
            return 2;
        case PM_ToolBarItemMargin:
            return 3;
        case PM_ButtonMargin:
            return 6;
        default:
            return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

void QtDocsStyle::drawPrimitive(PrimitiveElement element, const QStyleOption* option,
                                QPainter* painter, const QWidget* widget) const
{
    switch (element)
    {
        case PE_PanelButtonCommand:
        case PE_PanelButtonBevel:
            fillRounded(painter, option->rect, stateFill(option, aSurface), aOutline);
            return;

        // Toolbar buttons carry no border at rest; they only light up under the
        // pointer. This is the single biggest difference from the classic look.
        case PE_PanelButtonTool:
        case PE_IndicatorButtonDropDown:
        {
            const bool bActive
                = option->state & (State_Sunken | State_On | State_MouseOver);
            if (!bActive)
                return;
            fillRounded(painter, option->rect,
                        option->state & (State_Sunken | State_On) ? aAccentMuted : aSurfaceMuted);
            return;
        }

        case PE_PanelMenuBar:
        case PE_PanelToolBar:
        case PE_FrameMenu:
        case PE_FrameDockWidget:
        case PE_FrameStatusBarItem:
            painter->fillRect(option->rect, aSurface);
            return;

        // A focus ring that does not look like a dotted 1990s marquee.
        case PE_FrameFocusRect:
        {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(aAccent, 1));
            painter->drawRoundedRect(QRectF(option->rect).adjusted(0.5, 0.5, -0.5, -0.5), fRadius,
                                     fRadius);
            painter->restore();
            return;
        }

        case PE_Frame:
        case PE_FrameLineEdit:
            fillRounded(painter, option->rect, aSurface, aOutline);
            return;

        default:
            break;
    }
    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void QtDocsStyle::drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                              const QWidget* widget) const
{
    switch (element)
    {
        case CE_ToolBar:
        case CE_MenuBarEmptyArea:
            painter->fillRect(option->rect, aSurface);
            return;

        case CE_MenuBarItem:
        {
            painter->fillRect(option->rect, aSurface);
            if (option->state & (State_Selected | State_Sunken))
                fillRounded(painter, option->rect.adjusted(2, 2, -2, -2), aSurfaceMuted);
            if (const auto* pItem = qstyleoption_cast<const QStyleOptionMenuItem*>(option))
            {
                painter->save();
                painter->setPen(option->state & State_Enabled ? aText : aTextDisabled);
                painter->drawText(option->rect, Qt::AlignCenter | Qt::TextShowMnemonic,
                                  pItem->text);
                painter->restore();
            }
            return;
        }

        // Separators as hairlines rather than engraved grooves.
        case CE_ToolBoxTabShape:
            break;

        default:
            break;
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */

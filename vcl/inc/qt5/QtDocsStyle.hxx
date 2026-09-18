/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QtWidgets/QProxyStyle>

/**
 * A flat, rounded widget style for the WebAssembly build.
 *
 * The wasm build draws every widget itself: there is no desktop theme behind
 * it, so whatever QStyle is installed *is* the look of the application. The
 * default (Fusion) still paints the beveled, high-contrast frames of a classic
 * desktop toolkit, which looks dated next to a web application.
 *
 * This style keeps the geometry of Fusion -- LibreOffice asks the style for
 * sizes in many places, and changing them breaks layout in subtle ways -- and
 * only replaces how things are *painted*: flat fills, rounded corners, and a
 * restrained palette.
 *
 * It is installed unconditionally on EMSCRIPTEN (see QtCustomStyle::LoadCustomStyle).
 */
class QtDocsStyle : public QProxyStyle
{
public:
    QtDocsStyle();

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter,
                       const QWidget* widget = nullptr) const override;

    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter,
                     const QWidget* widget = nullptr) const override;

    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
                  const QWidget* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
                    const QWidget* widget = nullptr) const override;

    /** The palette this style is designed against. */
    static QPalette palette();
};

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */

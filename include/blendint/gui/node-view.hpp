/*
 * This file is part of BlendInt (a Blender-like Interface Library in
 * OpenGL).
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is free
 * software: you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * BlendInt (a Blender-like Interface Library in OpenGL) is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
 * Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BlendInt.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Contributor(s): Freeman Zhang <zhanggyb@gmail.com>
 */

#pragma once

#include <blendint/core/types.hpp>
#include <blendint/gui/abstract-node.hpp>
#include <blendint/gui/cubic-bezier-curve.hpp>
#include <blendint/gui/grid-guides.hpp>
#include <blendint/gui/abstract-scrollable.hpp>

namespace BlendInt {

  class NodeView: public AbstractScrollable
  {
  DISALLOW_COPY_AND_ASSIGN(NodeView);

  public:

    NodeView ();

    NodeView (int width, int height);

    virtual ~NodeView ();

    bool AddNode (AbstractNode* node);

    bool InsertNode (int index, AbstractNode* node);

    virtual bool IsExpandX () const;

    virtual bool IsExpandY () const;

    virtual Size GetPreferredSize () const;

    virtual bool SizeUpdateTest (const AbstractView* source,
                                 const AbstractView* target,
                                 int width,
                                 int height) const;

    virtual bool PositionUpdateTest (const AbstractView* source,
                                     const AbstractView* target,
                                     int x,
                                     int y) const;

    void SetFocusedNode (AbstractNode* node);

    static NodeView* GetNodeView (AbstractNode* node);

  protected:

    virtual void PerformSizeUpdate (const AbstractView* source,
                                    const AbstractView* target,
                                    int width,
                                    int height);

    virtual void PerformRoundTypeUpdate (int round_type);

    virtual void PerformRoundRadiusUpdate (float radius);

    virtual void PerformFocusOn (AbstractWindow* context);

    virtual void PerformFocusOff (AbstractWindow* context);

    virtual void PerformHoverIn (AbstractWindow* context);

    virtual void PerformHoverOut (AbstractWindow* context);

    virtual Response PerformKeyPress (AbstractWindow* context);

    virtual Response PerformMousePress (AbstractWindow* context);

    virtual Response PerformMouseRelease (AbstractWindow* context);

    virtual Response PerformMouseMove (AbstractWindow* context);

    virtual bool PreDraw (AbstractWindow* context);

    virtual Response Draw (AbstractWindow* context);

    virtual void PostDraw (AbstractWindow* context);

  private:

    void PerformMouseHover (AbstractWindow* context);

    void InitializeNodeView ();

    //CubicBezierCurve* curve_;

    GLuint vao_;

    GLBuffer<> vbo_;

    RefPtr<GridGuides> guides_;

    bool pressed_;

    bool focused_;

    bool hover_;

    bool mouse_tracking_record_;
  };

}

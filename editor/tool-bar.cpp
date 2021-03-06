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

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <blendint/gui/radio-button.hpp>
#include <blendint/gui/block.hpp>
#include <blendint/gui/tool-button.hpp>
#include <blendint/gui/linear-layout.hpp>
#include <blendint/gui/separator.hpp>
#include <blendint/gui/abstract-window.hpp>

#include "tool-bar.hpp"

namespace BlendInt {

  ToolBar::ToolBar(unsigned int color,
                   bool shaded,
                   short shadetop,
                   short shadedown)
  : AbstractFrame(FrameTopmost),
    layout_(0),
    focused_widget_(0),
    hovered_widget_(0),
    vao_(0),
    color_(color),
    shaded_(shaded),
    shadetop_(shadetop),
    shadedown_(shadedown),
    focused_(false),
    hover_(false),
    pressed_(false),
    cursor_position_(InsideRectangle),
    stack_(0)
  {
    layout_ = new LinearLayout(Vertical);

    PushBackSubView(layout_);
    RequestRedraw();

    set_size(layout_->size());

    InitializeToolBar();
    shadow_.reset(new FrameShadow(size(), RoundNone, 5.f));

    radio_group_.button_index_toggled().connect(this, &ToolBar::OnToggleStack);
  }

  ToolBar::~ToolBar ()
  {
    glDeleteVertexArrays(1, &vao_);
  }

  Size ToolBar::GetPreferredSize () const
  {
    return layout_->GetPreferredSize();
  }

  void ToolBar::LoadTools ()
  {
    layout_->AddWidget(CreateRadioLayout());

    stack_ = CreateStack();
    layout_->AddWidget(stack_);

    Resize(layout_->GetPreferredSize());
  }

  bool ToolBar::SizeUpdateTest (const AbstractView* source, const AbstractView* target, int width, int height)
  {
    if (target == this) {
      return true;
    }

    return false;
  }

  void ToolBar::PerformSizeUpdate (const AbstractView* source, const AbstractView* target, int width, int height)
  {
    if (target == this) {

      projection_matrix_ = glm::ortho(0.f,
                                      0.f + (float) width,
                                      0.f,
                                      0.f + (float) height,
                                      100.f, -100.f);

      set_size(width, height);

      if (view_buffer()) view_buffer()->Resize(size());

      std::vector<GLfloat> inner_verts;

      if (shaded_) {
        GenerateVertices(size(), 0, RoundNone, 0.f, Vertical,
                         shadetop_, shadedown_, &inner_verts, 0);
      } else {
        GenerateVertices(size(), 0, RoundNone, 0.f, &inner_verts,
                         0);
      }

      vbo_.bind(0);
      vbo_.set_sub_data(0, sizeof(GLfloat) * inner_verts.size(),
                        &inner_verts[0]);
      vbo_.reset();

      shadow_->Resize(size());

      ResizeSubView(layout_, size());
      RequestRedraw();
    }

    if (source == this) {
      report_size_update(source, target, width, height);
    }
  }

  bool ToolBar::PreDraw (AbstractWindow* context)
  {
    DeclareActiveFrame(context, this);

    if (refresh() && view_buffer()) {
      RenderToTexture(this, context, projection_matrix_, model_matrix_,
                               view_buffer()->texture());
    }

    return true;
  }

  Response ToolBar::Draw (AbstractWindow* context)
  {
    shadow_->Draw(position().x(), position().y());

    AbstractWindow::shaders()->frame_inner_program()->use();

    glUniform2f(
        AbstractWindow::shaders()->location(Shaders::FRAME_INNER_POSITION),
        position().x(), position().y());
    glUniform1i(AbstractWindow::shaders()->location(Shaders::FRAME_INNER_GAMMA),
                0);
    glUniform4fv(
        AbstractWindow::shaders()->location(Shaders::FRAME_INNER_COLOR), 1,
        color_.data());

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    if (view_buffer()) {

      AbstractWindow::shaders()->frame_image_program()->use();

      glUniform2f(
          AbstractWindow::shaders()->location(Shaders::FRAME_IMAGE_POSITION),
          position().x(), position().y());
      glUniform1i(
          AbstractWindow::shaders()->location(Shaders::FRAME_IMAGE_TEXTURE), 0);
      glUniform1i(
          AbstractWindow::shaders()->location(Shaders::FRAME_IMAGE_GAMMA), 0);

      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
      view_buffer()->Draw(0, 0);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    } else {

      glViewport(position().x(), position().y(), size().width(),
                 size().height());

      AbstractWindow::shaders()->SetWidgetProjectionMatrix(projection_matrix_);
      AbstractWindow::shaders()->SetWidgetModelMatrix(model_matrix_);

      DrawSubViewsOnce(context);

      glViewport(0, 0, context->size().width(), context->size().height());

    }

    return Finish;
  }

  void ToolBar::PostDraw (AbstractWindow* context)
  {
  }

  Response ToolBar::PerformMouseHover (AbstractWindow* context)
  {
    if (pressed_) return Finish;

    if (Contain(context->GetGlobalCursorPosition())) {

      cursor_position_ = InsideRectangle;

      if (!hover_) {
        PerformHoverIn(context);
      }

      AbstractWidget* new_hovered_widget = DispatchMouseHover(hovered_widget_,
                                                              context);

      if (new_hovered_widget != hovered_widget_) {

        if (hovered_widget_) {
          hovered_widget_->destroyed().disconnect1(
              this, &ToolBar::OnHoverWidgetDestroyed);
        }

        hovered_widget_ = new_hovered_widget;
        if (hovered_widget_) {
          hovered_widget_->destroyed().connect(this,
                            &ToolBar::OnHoverWidgetDestroyed);
        }

      }

      return Finish;

    } else {

      cursor_position_ = OutsideRectangle;

      if (hover_) {
        PerformHoverOut(context);
      }

      return Ignore;
    }
  }

  void ToolBar::PerformFocusOn (AbstractWindow* context)
  {
    focused_ = true;
  }

  void ToolBar::PerformFocusOff (AbstractWindow* context)
  {
    focused_ = false;

    if (hovered_widget_) {
      hovered_widget_->destroyed().disconnect1(
          this, &ToolBar::OnHoverWidgetDestroyed);
      ClearHoverWidgets(hovered_widget_, context);
      hovered_widget_ = 0;
    }

    if (focused_widget_) {
      focused_widget_->destroyed().disconnect1(
          this, &ToolBar::OnFocusedWidgetDestroyed);
      dispatch_focus_off(focused_widget_, context);
      focused_widget_ = 0;
    }
  }

  void ToolBar::PerformHoverIn (AbstractWindow* context)
  {
    hover_ = true;
  }

  void ToolBar::PerformHoverOut (AbstractWindow* context)
  {
    hover_ = false;

    if (hovered_widget_) {
      hovered_widget_->destroyed().disconnect1(
          this, &ToolBar::OnHoverWidgetDestroyed);
      ClearHoverWidgets(hovered_widget_, context);
      hovered_widget_ = 0;
    }
  }

  Response ToolBar::PerformKeyPress (AbstractWindow* context)
  {
    DeclareActiveFrame(context, this);

    Response response = Ignore;

    if (focused_widget_) {
      dispatch_key_press(focused_widget_, context);
    }

    return response;  }

  Response ToolBar::PerformMousePress (AbstractWindow* context)
  {
    DeclareActiveFrame(context, this);

    if (cursor_position_ == InsideRectangle) {

      if (context == super()) context->SetFocusedFrame(this);

      if (hovered_widget_) {

        AbstractView* widget = 0; // widget may be focused

        widget = RecursiveDispatchMousePress(hovered_widget_, context);

        if (widget == 0) {
          DBG_PRINT_MSG("%s", "widget 0");
          pressed_ = true;
        } else {
          SetFocusedWidget(dynamic_cast<AbstractWidget*>(widget), context);
        }

      } else {
        pressed_ = true;
        // SetFocusedWidget(0);
      }

      return Finish;

    } else {
      pressed_ = false;
      return Ignore;
    }
  }

  Response ToolBar::PerformMouseMove (AbstractWindow* context)
  {
    DeclareActiveFrame(context, this);

    Response retval = Ignore;
    if (pressed_) {
      if (focused_widget_) {
        retval = dispatch_mouse_move(focused_widget_, context);
      }
    } else {
      if(context->mouse_tracking()) {
        if (focused_widget_) {
          retval = dispatch_mouse_move(focused_widget_, context);
        }
      }
    }

    return retval;
  }

  Response ToolBar::PerformMouseRelease (AbstractWindow* context)
  {
    pressed_ = false;

    if (focused_widget_) {
      DeclareActiveFrame(context, this);
      return dispatch_mouse_release(focused_widget_, context);
    }

    return Ignore;
  }

  void ToolBar::InitializeToolBar ()
  {
    projection_matrix_ = glm::ortho(0.f, (float) size().width(), 0.f,
                                    (float) size().height(), 100.f, -100.f);
    model_matrix_ = glm::mat3(1.f);

    std::vector<GLfloat> inner_verts;

    if (shaded_) {
      GenerateVertices(size(), 0, RoundNone, 0.f, Vertical,
                       shadetop_, shadedown_, &inner_verts, 0);
    } else {
      GenerateVertices(size(), 0, RoundNone, 0.f, &inner_verts,
                       0);
    }

    vbo_.generate();
    glGenVertexArrays(1, &vao_);

    glBindVertexArray(vao_);
    vbo_.bind(0);
    vbo_.set_data(sizeof(GLfloat) * inner_verts.size(), &inner_verts[0]);
    glEnableVertexAttribArray(AttributeCoord);
    glVertexAttribPointer(AttributeCoord, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
    vbo_.reset();
  }

  LinearLayout* ToolBar::CreateRadioLayout ()
  {
    // Add widgets
    RadioButton* b1 = new RadioButton("Home");
    RadioButton* b2 = new RadioButton("Interface");
    RadioButton* b3 = new RadioButton("Theme");
    RadioButton* b4 = new RadioButton("Setting");
    RadioButton* b5 = new RadioButton("Help");

    radio_group_.AddButton(b1);
    radio_group_.AddButton(b2);
    radio_group_.AddButton(b3);
    radio_group_.AddButton(b4);
    radio_group_.AddButton(b5);

    Block* block = new Block(Horizontal);
    block->AddWidget(b1);
    block->AddWidget(b2);
    block->AddWidget(b3);
    block->AddWidget(b4);
    block->AddWidget(b5);

    LinearLayout* hlayout = new LinearLayout(Horizontal);
    hlayout->SetMargin(Margin(0, 0, 0, 0));
    Separator* sp1 = new Separator(true);
    Separator* sp2 = new Separator(true);

    hlayout->AddWidget(sp1);
    hlayout->AddWidget(block);
    hlayout->AddWidget(sp2);

    b1->SetChecked(true);

    return hlayout;
  }

  Stack* ToolBar::CreateStack()
  {
    Stack* stack = new Stack;

    LinearLayout* l1 = new LinearLayout(Horizontal, AlignLeft);
    ToolButton* btn1 = new ToolButton;
    btn1->SetAction(AbstractWindow::icons()->icon_32x32(Icons::MESH_CIRCLE), "Circle");
    ToolButton* btn2 = new ToolButton;
    btn2->SetAction(AbstractWindow::icons()->icon_32x32(Icons::MESH_CONE), "Cone");

    l1->AddWidget(btn1);
    l1->AddWidget(btn2);
    l1->AddWidget(new Separator(true));

    LinearLayout* l2 = new LinearLayout(Horizontal, AlignLeft);
    ToolButton* btn3 = new ToolButton;
    btn3->SetAction(AbstractWindow::icons()->icon_32x32(Icons::MESH_CUBE), "Cube");
    ToolButton* btn4 = new ToolButton;
    btn4->SetAction(AbstractWindow::icons()->icon_32x32(Icons::MESH_CYLINDER), "Cylinder");

    l2->AddWidget(btn3);
    l2->AddWidget(btn4);
    l2->AddWidget(new Separator(true));

    stack->AddWidget(l1);
    stack->AddWidget(l2);

    return stack;
  }

  void ToolBar::SetFocusedWidget (AbstractWidget* widget,
                                  AbstractWindow* context)
  {
    if (focused_widget_ == widget) return;

    if (focused_widget_) {
      dispatch_focus_off(focused_widget_, context);
      focused_widget_->destroyed().disconnect1(
          this, &ToolBar::OnFocusedWidgetDestroyed);
    }

    focused_widget_ = widget;
    if (focused_widget_) {
      dispatch_focus_on(focused_widget_, context);
      focused_widget_->destroyed().connect(this,
                        &ToolBar::OnFocusedWidgetDestroyed);
    }
  }

  void ToolBar::OnFocusedWidgetDestroyed (AbstractWidget* widget)
  {
    DBG_ASSERT(focused_widget_ == widget);

    //set_widget_focus_status(widget, false);
    DBG_PRINT_MSG("focused widget %s destroyed", widget->name().c_str());
    widget->destroyed().disconnect1(this, &ToolBar::OnFocusedWidgetDestroyed);

    focused_widget_ = 0;
  }

  void ToolBar::OnHoverWidgetDestroyed (AbstractWidget* widget)
  {
    DBG_ASSERT(hovered_widget_ == widget);

    DBG_PRINT_MSG("unset hover status of widget %s", widget->name().c_str());
    widget->destroyed().disconnect1(this, &ToolBar::OnHoverWidgetDestroyed);

    hovered_widget_ = 0;
  }

  void ToolBar::OnToggleStack (int index, bool toggled)
  {
    DBG_PRINT_MSG("switch widget in stack: %d", index);
    if(stack_) stack_->SetIndex(index);

    toggled_.Invoke(index, toggled);
  }

}

/* gobby - A GTKmm driven libobby client
 * Copyright (C) 2005 0x539 dev group
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "sourceview/sourceview.hpp"
#include "sourceview/private/sourceview_p.hpp"

// Initialize static member
Gtk::SourceView::CppClassType Gtk::SourceView::sourceview_class_;

const Glib::Class& Gtk::SourceView_Class::init()
{
	if(!gtype_)
	{
		class_init_func_ = &SourceView_Class::class_init_function;
		register_derived_type(gtk_source_view_get_type() );
	}

	return *this;
}

void Gtk::SourceView_Class::class_init_function(void* g_class, void* class_data)
{
	BaseClassType* const klass = static_cast<BaseClassType*>(g_class);
	CppClassParent::class_init_function(klass, class_data);

	// Hier irgendwelche Signale.
}

Glib::ObjectBase* Gtk::SourceView_Class::wrap_new(GObject* o)
{
	return manage(new SourceView( (GtkSourceView*)(o)));
}

Gtk::SourceView::SourceView()
 : Gtk::TextView(Glib::ConstructParams(sourceview_class_.init()))
{
}

Gtk::SourceView::SourceView(const Glib::ConstructParams& construct_params)
 : Gtk::TextView(construct_params)
{
}

Gtk::SourceView::SourceView(GtkSourceView* castitem)
 : Gtk::TextView(reinterpret_cast<GtkTextView*>(castitem) )
{
}

/*Gtk::SourceView::SourceView(const Glib::RefPtr<SourceBuffer>& buffer)
   Gtk::TextView(Glib::ConstructParams(sourceview_class_.init(), (char*)0))
{
	set_buffer(buffer);
}*/

Gtk::SourceView::~SourceView()
{
	// TODO: TextView-dtor calls destroy_ as well...
	destroy_();
}

GType Gtk::SourceView::get_type()
{
	return sourceview_class_.init().get_type();
}

GType Gtk::SourceView::get_base_type()
{
	return gtk_source_view_get_type();
}

/*Glib::RefPtr<Gtk::SourceBuffer> Gtk::SourceView::get_buffer()
{
	return Glib::RefPtr<Gtk::SourceBuffer>::cast_static(Gtk::TextView::get_buffer() );
}

Glib::RefPtr<const Gtk::SourceBuffer> Gtk::SourceView::get_buffer() const
{
	return Glib::RefPtr<Gtk::SourceBuffer>::cast_static(Gtk::TextView::get_buffer() );
}
*/

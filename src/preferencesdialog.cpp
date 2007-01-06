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

#include <stdexcept>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>

#include <obby/format_string.hpp>

#include "common.hpp"
#include "preferencesdialog.hpp"

namespace
{
	bool lang_sort(const Glib::RefPtr<Gtk::SourceLanguage>& first,
	               const Glib::RefPtr<Gtk::SourceLanguage>& second)
	{
		return first->get_name() < second->get_name();
	}

	Gtk::ToolbarStyle rownum_to_toolstyle(int rownum)
	{
		switch(rownum)
		{
		case 0: return Gtk::TOOLBAR_TEXT;
		case 1: return Gtk::TOOLBAR_ICONS;
		case 2: default: return Gtk::TOOLBAR_BOTH;
		}
	}
}

Gobby::PreferencesDialog::Page::Page():
	Gtk::Frame()
{
	// Remove shadow - use the frame just as container
	set_shadow_type(Gtk::SHADOW_NONE);
	set_border_width(10);
}

Gobby::PreferencesDialog::Editor::Editor(const Preferences& preferences,
                                         Gtk::Tooltips& tooltips):
	m_frame_tab(_("Tab Stops") ),
	m_frame_indentation(_("Indentation") ),
	m_frame_homeend(_("Home/End behaviour") ),
	m_lbl_tab_width(_("Tab width:"), Gtk::ALIGN_RIGHT),
	m_btn_tab_spaces(_("Insert spaces instead of tabs") ),
	m_btn_indentation_auto(_("Enable automatic indentation") ),
	m_btn_homeend_smart(_("Smart home/end") )
{
	unsigned int tab_width = preferences.editor.tab_width;
	bool tab_spaces = preferences.editor.tab_spaces;
	bool indentation_auto = preferences.editor.indentation_auto;
	bool homeend_smart = preferences.editor.homeend_smart;

	m_ent_tab_width.set_range(1, 8);
	m_ent_tab_width.set_value(tab_width);
	m_ent_tab_width.set_increments(1, 1);

	// TODO: Improve this description
	tooltips.set_tip(m_btn_homeend_smart,
		_("With this option enabled, Home/End keys move to first/last "
		  "character before going to the start/end of the line.") );

	m_box_tab_width.set_spacing(5);
	m_box_tab_width.pack_start(m_lbl_tab_width, Gtk::PACK_SHRINK);
	m_box_tab_width.pack_start(m_ent_tab_width, Gtk::PACK_EXPAND_WIDGET);

	m_btn_tab_spaces.set_active(tab_spaces);
	m_btn_indentation_auto.set_active(indentation_auto);
	m_btn_homeend_smart.set_active(homeend_smart);

	m_box_tab.set_spacing(5);
	m_box_tab.set_border_width(5);
	m_box_tab.pack_start(m_box_tab_width, Gtk::PACK_SHRINK);
	m_box_tab.pack_start(m_btn_tab_spaces, Gtk::PACK_SHRINK);

	m_box_indentation.set_spacing(5);
	m_box_indentation.set_border_width(5);
	m_box_indentation.pack_start(m_btn_indentation_auto, Gtk::PACK_SHRINK);

	m_box_homeend.set_spacing(5);
	m_box_homeend.set_border_width(5);
	m_box_homeend.pack_start(m_btn_homeend_smart, Gtk::PACK_SHRINK);

	m_frame_tab.add(m_box_tab);
	m_frame_indentation.add(m_box_indentation);
	m_frame_homeend.add(m_box_homeend);

	m_box.set_spacing(5);
	m_box.pack_start(m_frame_tab, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_indentation, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_homeend, Gtk::PACK_SHRINK);

	add(m_box);
}

void Gobby::PreferencesDialog::Editor::set(Preferences::Editor& editor) const
{
	editor.tab_width = m_ent_tab_width.get_value_as_int();
	editor.tab_spaces = m_btn_tab_spaces.get_active();
	editor.indentation_auto = m_btn_indentation_auto.get_active();
	editor.homeend_smart = m_btn_homeend_smart.get_active();
}

Gobby::PreferencesDialog::View::View(const Preferences& preferences):
	m_frame_wrap(_("Text wrapping") ),
	m_frame_linenum(_("Line numbers") ),
	m_frame_curline(_("Current line") ),
	m_frame_margin(_("Right margin") ),
	m_frame_bracket(_("Bracket matching") ),
	m_btn_wrap_text(_("Enable text wrapping") ),
	m_btn_wrap_words(_("Do not split words over two lines") ),
	m_btn_linenum_display(_("Display line numbers") ),
	m_btn_curline_highlight(_("Highlight current line") ),
	m_btn_margin_display(_("Display right margin") ),
	m_lbl_margin_pos(_("Right margin at column:") ),
	m_btn_bracket_highlight(_("Highlight matching bracket") )
{
	bool wrap_text = preferences.view.wrap_text;
	bool wrap_words = preferences.view.wrap_words;
	bool linenum_display = preferences.view.linenum_display;
	bool curline_highlight = preferences.view.curline_highlight;
	bool margin_display = preferences.view.margin_display;
	unsigned int margin_pos = preferences.view.margin_pos;
	bool bracket_highlight = preferences.view.bracket_highlight;

	m_btn_margin_display.signal_toggled().connect(
		sigc::mem_fun(*this, &View::on_margin_display_toggled) );

	m_ent_margin_pos.set_range(1, 1024);
	m_ent_margin_pos.set_value(margin_pos);
	m_ent_margin_pos.set_increments(1, 16);

	m_btn_wrap_text.set_active(wrap_text);
	m_btn_wrap_words.set_active(!wrap_words);
	m_btn_linenum_display.set_active(linenum_display);
	m_btn_curline_highlight.set_active(curline_highlight);
	m_btn_margin_display.set_active(margin_display);
	m_btn_bracket_highlight.set_active(bracket_highlight);

	m_box_margin_pos.set_spacing(5);
	m_box_margin_pos.pack_start(m_lbl_margin_pos, Gtk::PACK_SHRINK);
	m_box_margin_pos.pack_start(m_ent_margin_pos, Gtk::PACK_EXPAND_WIDGET);
	m_box_wrap.set_spacing(5);
	m_box_wrap.set_border_width(5);
	m_box_wrap.pack_start(m_btn_wrap_text, Gtk::PACK_SHRINK);
	m_box_wrap.pack_start(m_btn_wrap_words, Gtk::PACK_SHRINK);

	m_box_linenum.set_spacing(5);
	m_box_linenum.set_border_width(5);
	m_box_linenum.pack_start(m_btn_linenum_display, Gtk::PACK_SHRINK);

	m_box_curline.set_spacing(5);
	m_box_curline.set_border_width(5);
	m_box_curline.pack_start(m_btn_curline_highlight, Gtk::PACK_SHRINK);

	m_box_margin.set_spacing(5);
	m_box_margin.set_border_width(5);
	m_box_margin.pack_start(m_btn_margin_display, Gtk::PACK_SHRINK);
	m_box_margin.pack_start(m_box_margin_pos, Gtk::PACK_SHRINK);

	m_box_bracket.set_spacing(5);
	m_box_bracket.set_border_width(5);
	m_box_bracket.pack_start(m_btn_bracket_highlight, Gtk::PACK_SHRINK);
	
	m_frame_wrap.add(m_box_wrap);
	m_frame_linenum.add(m_box_linenum);
	m_frame_curline.add(m_box_curline);
	m_frame_margin.add(m_box_margin);
	m_frame_bracket.add(m_box_bracket);

	m_box.set_spacing(5);
	m_box.pack_start(m_frame_wrap, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_linenum, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_curline, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_margin, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_bracket, Gtk::PACK_SHRINK);

	add(m_box);
}

void Gobby::PreferencesDialog::View::set(Preferences::View& view) const
{
	view.wrap_text = m_btn_wrap_text.get_active();
	view.wrap_words = !m_btn_wrap_words.get_active();
	view.linenum_display = m_btn_linenum_display.get_active();
	view.curline_highlight = m_btn_curline_highlight.get_active();
	view.margin_display = m_btn_margin_display.get_active();
	view.margin_pos = m_ent_margin_pos.get_value_as_int();
	view.bracket_highlight = m_btn_bracket_highlight.get_active();
}

void Gobby::PreferencesDialog::View::on_margin_display_toggled()
{
	m_box_margin_pos.set_sensitive(m_btn_margin_display.get_active() );
}

Gobby::PreferencesDialog::Appearance::
	Appearance(const Gobby::Preferences& preferences):
	m_frame_toolbar(_("Toolbar") ),
	m_frame_windows(_("Windows") ),
	m_btn_remember(_("Remember the positions and states") )
{
	Gtk::ToolbarStyle style = preferences.appearance.toolbar_show;
	bool remember = preferences.appearance.remember;

	m_cmb_toolbar_style.append_text(_("Show text only") );
	m_cmb_toolbar_style.append_text(_("Show icons only") );
	m_cmb_toolbar_style.append_text(_("Show both icons and text") );

	switch(style)
	{
	case Gtk::TOOLBAR_TEXT: m_cmb_toolbar_style.set_active(0); break;
	case Gtk::TOOLBAR_ICONS: m_cmb_toolbar_style.set_active(1); break;
	case Gtk::TOOLBAR_BOTH: m_cmb_toolbar_style.set_active(2); break;
	}

	m_box_toolbar.set_spacing(5);
	m_box_toolbar.set_border_width(5);
	m_box_toolbar.pack_start(m_cmb_toolbar_style, Gtk::PACK_SHRINK);

	m_frame_toolbar.add(m_box_toolbar);

	m_box_windows.set_spacing(5);
	m_box_windows.set_border_width(5);
	m_btn_remember.set_active(remember);
	m_box_windows.pack_start(m_btn_remember, Gtk::PACK_SHRINK);

	m_frame_windows.add(m_box_windows);

	m_box.set_spacing(5);
	m_box.pack_start(m_frame_toolbar, Gtk::PACK_SHRINK);
	m_box.pack_start(m_frame_windows, Gtk::PACK_SHRINK);

	add(m_box);
}

void Gobby::PreferencesDialog::Appearance::
	set(Preferences::Appearance& appearance) const
{
	appearance.toolbar_show = rownum_to_toolstyle(
		m_cmb_toolbar_style.get_active_row_number()
	);

	appearance.remember = m_btn_remember.get_active();
}

Gobby::PreferencesDialog::FileList::LanguageColumns::LanguageColumns()
{
	add(language);
	add(language_name);
}

Gobby::PreferencesDialog::FileList::FileColumns::FileColumns()
{
	add(pattern);
	add(mime_type);
	add(language);
}

Gobby::PreferencesDialog::FileList::FileList(Gtk::Window& parent,
                                             const Preferences& preferences,
                                             const LangManager& lang_mgr):
	m_parent(parent), m_lang_mgr(lang_mgr),
	m_viewcol_pattern(_("Pattern"), file_columns.pattern),
	m_viewcol_lang(_("Language"), m_renderer_lang),
	m_viewcol_mimetype(_("Mime type"), file_columns.mime_type),
	m_intro(
		_("This is a list of all recognized file types"),
		Gtk::ALIGN_LEFT
	),
	m_hbox(Gtk::BUTTONBOX_END, 12),
	m_btn_add(Gtk::Stock::ADD), m_btn_remove(Gtk::Stock::REMOVE),
	m_lang_list(Gtk::ListStore::create(lang_columns) ),
	m_file_list(Gtk::ListStore::create(file_columns) )
{
	// Create language list
	std::list<Glib::RefPtr<Gtk::SourceLanguage> > languages =
		lang_mgr->get_available_languages();

	languages.sort(&lang_sort);

	for(std::list<Glib::RefPtr<Gtk::SourceLanguage> >::const_iterator iter =
		languages.begin();
	    iter != languages.end();
	    ++ iter)
	{
		Gtk::TreeIter tree_it = m_lang_list->append();
		(*tree_it)[lang_columns.language] = *iter;
		(*tree_it)[lang_columns.language_name] = (*iter)->get_name();

		m_lang_map[*iter] = tree_it;
	}

	m_renderer_pattern = static_cast<Gtk::CellRendererText*>(
		m_viewcol_pattern.get_first_cell_renderer()
	);

	m_renderer_mimetype = static_cast<Gtk::CellRendererText*>(
		m_viewcol_mimetype.get_first_cell_renderer()
	);

	m_renderer_pattern->property_editable() = true;
	m_renderer_pattern->signal_edited().connect(
		sigc::mem_fun(*this, &FileList::on_pattern_edited)
	);

	m_renderer_mimetype->property_editable() = true;
	m_renderer_mimetype->signal_edited().connect(
		sigc::mem_fun(*this, &FileList::on_mimetype_edited)
	);

	m_renderer_lang.property_has_entry() = false;
	m_renderer_lang.property_model() = m_lang_list;
	m_renderer_lang.property_text_column() = 1;
	m_renderer_lang.property_editable() = true;
	m_renderer_lang.signal_edited().connect(
		sigc::mem_fun(*this, &FileList::on_language_edited)
	);

	m_viewcol_lang.set_cell_data_func(
		m_renderer_lang,
		sigc::mem_fun(*this, &FileList::cell_data_file_language)
	);


	m_viewcol_pattern.set_sort_column(file_columns.pattern);
	//m_viewcol_lang.set_sort_column(file_columns.language);
	m_viewcol_mimetype.set_sort_column(file_columns.mime_type);

	const Preferences::FileList& filelist = preferences.files;

	for(Preferences::FileList::iterator iter = filelist.begin();
	    iter != filelist.end();
	    ++ iter)
	{
		std::list<Glib::ustring> mime_types =
			iter.language()->get_mime_types();

		Gtk::TreeIter tree_it = m_file_list->append();
		(*tree_it)[file_columns.pattern] = iter.pattern();
		set_language(tree_it, iter.language() );
	}

	m_view.set_model(m_file_list);

	m_view.append_column(m_viewcol_pattern);
	m_view.append_column(m_viewcol_lang);
	m_view.append_column(m_viewcol_mimetype);

	m_view.get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
	m_view.get_selection()->signal_changed().connect(
		sigc::mem_fun(*this, &FileList::on_selection_changed)
	);

	m_view.set_rules_hint(true);

	m_btn_add.signal_clicked().connect(
		sigc::mem_fun(*this, &FileList::on_file_add)
	);

	m_btn_remove.signal_clicked().connect(
		sigc::mem_fun(*this, &FileList::on_file_remove)
	);

	m_hbox.add(m_btn_remove);
	m_hbox.add(m_btn_add);

	m_wnd.add(m_view);
	m_wnd.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_wnd.set_shadow_type(Gtk::SHADOW_IN);

	m_vbox.pack_start(m_intro, Gtk::PACK_SHRINK);
	m_vbox.pack_start(m_wnd, Gtk::PACK_EXPAND_WIDGET);
	m_vbox.pack_start(m_hbox, Gtk::PACK_SHRINK);
	m_vbox.set_spacing(8);

	add(m_vbox);

	on_selection_changed();
}

void Gobby::PreferencesDialog::FileList::set(Preferences::FileList& files) const
{
	Gtk::TreeNodeChildren children = m_file_list->children();
	for(Gtk::TreeIter iter = children.begin();
	    iter != children.end();
	    ++ iter)
	{
		Gtk::TreeIter lang_it = (*iter)[file_columns.language];
		files.add(
			(*iter)[file_columns.pattern],
			(*lang_it)[lang_columns.language]
		);
	}
}

void Gobby::PreferencesDialog::FileList::
	cell_data_file_language(Gtk::CellRenderer* renderer,
	                        const Gtk::TreeIter& iter)
{
	Gtk::TreeIter lang_it = (*iter)[file_columns.language];
	static_cast<Gtk::CellRendererText*>(renderer)->property_text() =
		(*lang_it)[lang_columns.language_name];
}

void Gobby::PreferencesDialog::FileList::
	on_pattern_edited(const Glib::ustring& path,
	                  const Glib::ustring& new_text)
{
	if(new_text.empty() )
	{
		Gtk::MessageDialog dlg(
			m_parent,
			_("Pattern must not be empty."),
			false,
			Gtk::MESSAGE_ERROR,
			Gtk::BUTTONS_OK,
			true
		);

		dlg.run();

		// TODO: Take iterator at beginning and remove here back to
		// path to avoid borkage
		/*m_view.set_cursor(
			Gtk::TreePath(path),
			m_viewcol_pattern,
			true
		);*/
	}
	else
	{
		Gtk::TreeIter iter = m_file_list->get_iter(Gtk::TreePath(path));
		(*iter)[file_columns.pattern] = new_text;
	}
}

void Gobby::PreferencesDialog::FileList::
	on_mimetype_edited(const Glib::ustring& path,
	                   const Glib::ustring& new_text)
{
	Glib::RefPtr<Gtk::SourceLanguage> lang =
		m_lang_mgr->get_language_from_mime_type(new_text);

	if(!lang)
	{
		obby::format_string str(
			_("There is no language with the mime type '%0%'.")
		);

		str << new_text.raw();

		Gtk::MessageDialog dlg(
			m_parent,
			str.str(),
			false,
			Gtk::MESSAGE_ERROR,
			Gtk::BUTTONS_OK,
			true
		);

		dlg.run();
	}
	else
	{
		set_language(m_file_list->get_iter(Gtk::TreePath(path)), lang);
	}
}

void Gobby::PreferencesDialog::FileList::
	on_language_edited(const Glib::ustring& path,
	                   const Glib::ustring& new_text)
{
	// We do not get an iterator/path/whatever that points to the
	// chosen language in the language list.
	Glib::RefPtr<Gtk::SourceLanguage> lang(NULL);
	Gtk::TreeNodeChildren children = m_lang_list->children();

	for(Gtk::TreeIter iter = children.begin();
	    iter != children.end();
	    ++ iter)
	{
		if( (*iter)[lang_columns.language_name] == new_text)
		{
			lang = (*iter)[lang_columns.language];
			break;
		}
	}

	if(!lang)
	{
		// The language must exist since we added all available
		// languages to that list
		throw std::logic_error(
			"Gobby::PreferencesDialog::FileList::"
			"on_language_edited:\n"
			"Chosen language is not in language list"
		);
	}

	set_language(m_file_list->get_iter(Gtk::TreePath(path)), lang);
}

void Gobby::PreferencesDialog::FileList::on_selection_changed()
{
	std::list<Gtk::TreePath> list =
		m_view.get_selection()->get_selected_rows();

	m_btn_remove.set_sensitive(list.begin() != list.end() );
}

void Gobby::PreferencesDialog::FileList::on_file_add()
{
	Gtk::TreeIter iter = m_file_list->append();
	set_language(iter, m_lang_map.begin()->first);

	m_view.set_cursor(
		m_file_list->get_path(iter),
		m_viewcol_pattern,
		true
	);
}

void Gobby::PreferencesDialog::FileList::on_file_remove()
{
	std::list<Gtk::TreePath> list =
		m_view.get_selection()->get_selected_rows();

	std::list<Gtk::TreeIter> iter_list;

	// Path offsets get borked when removing multiple rows, so we
	// convert all paths to iterators before
	for(std::list<Gtk::TreePath>::const_iterator iter = list.begin();
	    iter != list.end();
	    ++ iter)
	{
		iter_list.push_back(m_file_list->get_iter(*iter) );
	}

	for(std::list<Gtk::TreeIter>::const_iterator iter = iter_list.begin();
	    iter != iter_list.end();
	    ++ iter)
	{
		m_file_list->erase(*iter);
	}
}

void Gobby::PreferencesDialog::FileList::set_language(const Gtk::TreeIter& row,
                                                      const Language& lang)
{
	map_type::const_iterator lang_it = m_lang_map.find(lang);
	if(lang_it == m_lang_map.end() )
	{
		throw std::logic_error(
			"Gobby::PreferencesDialog::FileList::set_language:\n"
			"Given language is not in language map"
		);
	}

	std::list<Glib::ustring> mime_types = lang->get_mime_types();

	(*row)[file_columns.language] = lang_it->second;
	(*row)[file_columns.mime_type] = mime_types.front();
}

Gobby::PreferencesDialog::PreferencesDialog(Gtk::Window& parent,
                                            const Preferences& preferences,
                                            const LangManager& lang_mgr,
                                            bool local)
 : Gtk::Dialog(_("Preferences"), parent, true),
   m_page_editor(preferences, m_tooltips), m_page_view(preferences),
   m_page_appearance(preferences), m_page_files(*this, preferences, lang_mgr)
{
	m_notebook.append_page(m_page_editor, _("Editor") );
	m_notebook.append_page(m_page_view, _("View") );

	// Appearance only affects the global Gobby window
	if(!local) m_notebook.append_page(m_page_appearance, _("Appearance") );
	if(!local) m_notebook.append_page(m_page_files, _("Files") );

	get_vbox()->set_spacing(5);
	get_vbox()->pack_start(m_notebook, Gtk::PACK_EXPAND_WIDGET);

	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	set_border_width(10);
	set_default_size(350, 400);
	//set_resizable(false);

	show_all();
}

void Gobby::PreferencesDialog::set(Preferences& preferences) const
{
	m_page_editor.set(preferences.editor);
	m_page_view.set(preferences.view);
	m_page_appearance.set(preferences.appearance);
	m_page_files.set(preferences.files);
}

#if 0
const Gobby::PreferencesDialog::Editor&
Gobby::PreferencesDialog::editor() const
{
	return m_page_editor;
}

const Gobby::PreferencesDialog::View&
Gobby::PreferencesDialog::view() const
{
	return m_page_view;
}

const Gobby::PreferencesDialog::Appearance&
Gobby::PreferencesDialog::appearance() const
{
	return m_page_appearance;
}
#endif

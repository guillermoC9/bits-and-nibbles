/*
    test_pdf.c

    Test of pdf routines

    (CC) Creative Commons 2025 by Guillermo Amodeo Ojeda.

    I dedicate the software in this file to the public domain, so it is entirely free.
    Thus you may use it and/or redistribute it for whatever purpose you may choose.

    It is actually distributed with the hope that it would be useful to someone,
    but with no guarantee whatsoever. Not even the guarantee that it will work.

    Also, being free software, it is provided without warranty of any kind,
    not even the implied warranty of merchantability or fitness for a particular
    purpose. I also disclaim any liability arising from any use of this software.

    Read the CC license at https://creativecommons.org/publicdomain/zero/1.0/

    NOTES:

        - Written by Guillermo Amodeo Ojeda.                  
            
                        --oO0Oo--
*/
#include "pdf.h"

/* -------------------------------------- */
/* -------------------------------------- */

typedef struct {
    const wchar_t *name;
    const wchar_t *capital;
    int         area;
    int         population;
} pdf_country_t;

pdf_country_t countries[] =
{
    {L"Austria",L"Vienna",83859,8075},
    {L"Belgium",L"Brussels",30518,10192},
    {L"Denmark",L"Copenhagen",43094,5295},
    {L"Finland",L"Helsinki",304529,5147},
    {L"France",L"Paris",543965,58728},
    {L"Germany",L"Berlin",357022,82057},
    {L"Greece",L"Athens",131625,10511},
    {L"Ireland",L"Dublin",70723,3694},
    {L"Italy",L"Roma",301316,57563},
    {L"Luxembourg",L"Luxembourg",2586,424},
    {L"Netherlands",L"Amsterdam",41526,15654},
    {L"Portugal",L"Lisbon",91906,9957},
    {L"Spain",L"Madrid",504790,39348},
    {L"Sweden",L"Stockholm",410934,8839},
    {L"United Kingdom",L"London",243820,58862},
    {NULL,NULL,0,0}
};

const wchar_t *headers[] = {L"Country",L"Capital",L"Area (sq km)",L"Pop. (thousands)"};

/* -------------------------------------- */

void pdf_basic_table(pdf_t *pdf)
{
    int t;

    pdf_set_font(pdf,"arial",PDF_FNT_REGULAR,14);
    pdf_set_line_style(pdf,PDF_SOLID_LINE);
    for(t=0;t<4;t++)
        pdf_draw_cell(pdf,45,7,PDF_A_FULL_B,0,headers[t]);

    pdf_line_feed(pdf);

    for(t=0; countries[t].name; t++)
    {
        pdf_draw_cell(pdf,45,6,PDF_A_FULL_B,0,countries[t].name);
        pdf_draw_cell(pdf,45,6,PDF_A_FULL_B,0,countries[t].capital);
        pdf_draw_cell(pdf,45,6,PDF_A_FULL_B,0,L"%d",countries[t].area);
        pdf_draw_cell(pdf,45,6,PDF_A_FULL_B,0,L"%d",countries[t].population);
        pdf_line_feed(pdf);
    }
}


/* -------------------------------------- */

void pdf_improved_table(pdf_t *pdf)
{
    int t,attr;
    float w[4]={40,35,40,45},sum = 0;
    wchar_t tmp[128];

    pdf_set_font(pdf,"arial",PDF_FNT_BOLD,14);
    pdf_set_line_style(pdf,PDF_SOLID_LINE);
    for(t=0;t<4;t++)
    {
        pdf_draw_cell(pdf,w[t],7,PDF_A_FULL_B|PDF_A_CENTRE_J,0,headers[t]);
        sum += w[t];
    }
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_line_feed(pdf);
    attr = PDF_A_LFT_B|PDF_A_RGT_B;
    for(t=0; countries[t].name; t++)
    {

        pdf_draw_cell(pdf,w[0],6,attr|PDF_A_LEFT_J,0,countries[t].name);
        pdf_draw_cell(pdf,w[1],6,attr|PDF_A_LEFT_J,0,countries[t].capital);
        pdf_draw_cell(pdf,w[2],6,attr|PDF_A_RIGHT_J,0,pdf_format_num(tmp,128,L',',0,0,countries[t].area,0));
        pdf_draw_cell(pdf,w[3],6,attr|PDF_A_RIGHT_J,0,pdf_format_num(tmp,128,L',',0,0,countries[t].population,0));
        pdf_line_feed(pdf);
    }
    pdf_draw_cell(pdf,sum,2,PDF_A_TOP_B,0,NULL);
    pdf_line_feed(pdf);
}


/* -------------------------------------- */

void pdf_fancy_table(pdf_t *pdf,float k)
{
    int t,attr1,attr2,attr;
    float w[4]={40,35,40,45},sum = 0, f;
    wchar_t tmp[128],h = 6;

    if(k == 0)
        k = 1;

    f = pdf_get_font_size_for_height(pdf,5);

    h*=k;
    f*=k;

    pdf_set_fill_color(pdf,102,107,175);
    pdf_set_text_color(pdf,255,255,255);
    pdf_set_draw_color(pdf,0,0,128);
	pdf_set_line_width(pdf,0.3 * k);
    pdf_set_font(pdf,"arial",PDF_FNT_BOLD,f);
    pdf_set_line_style(pdf,PDF_SOLID_LINE);
    attr = PDF_A_FULL_B|PDF_A_FILLED|PDF_A_CENTRE_J|PDF_A_FIT_TEXT;
    for(t=0;t<4;t++)
    {
        w[t] = w[t]*k;
        if(t == 0)
            attr1 = attr | PDF_A_RND_TL;
        else if (t==3)
            attr1 = attr | PDF_A_RND_TR;
        else
            attr1 = attr;

        pdf_draw_cell(pdf,w[t],7 * k,attr1,0,headers[t]);
        sum += w[t];
    }
    pdf_set_fill_color(pdf,224,235,255);
    pdf_set_text_color(pdf,0,0,128);
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_line_feed(pdf);
    attr1 = PDF_A_LFT_B|PDF_A_RGT_B|PDF_A_FIT_TEXT;
    attr2 = attr1|PDF_A_FILLED;
    attr  = attr1;
    for(t=0; countries[t].name; t++)
    {
        pdf_draw_cell(pdf,w[0],h,attr|PDF_A_LEFT_J,0,countries[t].name);
        pdf_draw_cell(pdf,w[1],h,attr|PDF_A_LEFT_J,0,countries[t].capital);
        pdf_draw_cell(pdf,w[2],h,attr|PDF_A_RIGHT_J,0,pdf_format_num(tmp,128,L',',0,0,countries[t].area,0));
        pdf_draw_cell(pdf,w[3],h,attr|PDF_A_RIGHT_J,0,pdf_format_num(tmp,128,L',',0,0,countries[t].population,0));
        pdf_line_feed(pdf);
        attr = (attr == attr1) ? attr2 : attr1;
    }
    pdf_draw_cell(pdf,sum,2 * k,PDF_A_TOP_B,0,NULL);
    pdf_line_feed(pdf);
}


/* -------------------------------------- */
/* -------------------------------------- */

const wchar_t *pdf_20k_chapter1= L"The year 1866 was marked by a bizarre development, an unexplained and downright inexplicable "
                                 L"phenomenon that surely no one has forgotten. Without getting into those rumors that upset civilians "
                                 L"in the seaports and deranged the public mind even far inland, it must be said that professional seamen "
                                 L"were especially alarmed. Traders, shipowners, captains of vessels, skippers, and master mariners from "
                                 L"Europe and America, naval officers from every country, and at their heels the various national governments"
                                 L" on these two continents, were all extremely disturbed by the business.\nIn essence, over a period of time"
                                 L" several ships had encountered 'an enormous thing' at sea, a long spindle-shaped object, sometimes giving "
                                 L"off a phosphorescent glow, infinitely bigger and faster than any whale.\nThe relevant data on this apparition"
                                 L", as recorded in various logbooks, agreed pretty closely as to the structure of the object or creature in "
                                 L"question, its unprecedented speed of movement, its startling locomotive power, and the unique vitality with"
                                 L"which it seemed to be gifted.  If it was a cetacean, it exceeded in bulk any whale previously classified by "
                                 L"science.  No naturalist, neither Cuvier nor Lacépède, neither Professor Dumeril nor Professor de Quatrefages,"
                                 L"would have accepted the existence of such a monster sight unseen -- specifically, unseen by their own scientific"
                                 L" eyes.\nStriking an average of observations taken at different times -- rejecting those timid estimates that "
                                 L"gave the object a length of 200 feet, and ignoring those exaggerated views that saw it as a mile wide and three"
                                 L" long--you could still assert that this phenomenal creature greatly exceeded the dimensions of anything then "
                                 L"known to ichthyologists, if it existed at all.\nNow then, it did exist, this was an undeniable fact; and since"
                                 L" the human mind dotes on objects of wonder, you can understand the worldwide excitement caused by this unearthly"
                                 L" apparition. As for relegating it to the realm of fiction, that charge had to be dropped.\nIn essence, on July "
                                 L"20, 1866, the steamer Governor Higginson, from the Calcutta & Burnach Steam Navigation Co., encountered this "
                                 L"moving mass five miles off the eastern shores of Australia. Captain Baker at first thought he was in the "
                                 L"presence of an unknown reef; he was even about to fix its exact position when two waterspouts shot out of this"
                                 L"inexplicable object and sprang hissing into the air some 150 feet. So, unless this reef was subject to the "
                                 L"intermittent eruptions of a geyser, the Governor Higginson had fair and honest dealings with some aquatic mammal"
                                 L", until then unknown, that could spurt from its blowholes waterspouts mixed with air and steam.\nSimilar events"
                                 L" were likewise observed in Pacific seas, on July 23 of the same year, by the Christopher Columbus from the West "
                                 L"India & Pacific Steam Navigation Co.  Consequently, this extraordinary cetacean could transfer itself from one "
                                 L"locality to another with startling swiftness, since within an interval of just three days, the Governor Higginson "
                                 L"and the Christopher Columbus had observed it at two positions on the charts separated by a distance of more "
                                 L"than 700 nautical leagues.\nFifteen days later and 2,000 leagues farther, the Helvetia from the Compagnie "
                                 L"Nationale and the Shannon from the Royal Mail line, running on opposite tacks in that part of the Atlantic "
                                 L"lying between the United States and Europe, respectively signaled each other that the monster had been sighted"
                                 L" in latitude 42 degrees 15' north and longitude 60 degrees 35' west of the meridian of Greenwich. From their"
                                 L" simultaneous observations, they were able to estimate the mammal's minimum length at more than 350 English "
                                 L"feet; this was because both the Shannon and the Helvetia were of smaller dimensions, although each measured "
                                 L"100 meters stem to stern. Now then, the biggest whales, those rorqual whales that frequent the waterways of "
                                 L"the Aleutian Islands, have never exceeded a length of 56 meters--if they reach even that.\nOne after another, "
                                 L"reports arrived that would profoundly affect public opinion:  new observations taken by the transatlantic liner"
                                 L" Pereire, the Inman line's Etna running afoul of the monster, an official report drawn up by officers on the "
                                 L"French frigate Normandy, dead-earnest reckonings obtained by the general staff of Commodore Fitz-James aboard "
                                 L"the Lord Clyde. In lighthearted countries, people joked about this phenomenon, but such serious, practical "
                                 L"countries as England, America, and Germany were deeply concerned.\nIn every big city the monster was the latest"
                                 L" rage; they sang about it in the coffee houses, they ridiculed it in the newspapers, they dramatized it in the "
                                 L"theaters.  The tabloids found it a fine opportunity for hatching all sorts of hoaxes. In those newspapers short "
                                 L"of copy, you saw the reappearance of every gigantic imaginary creature, from 'Moby Dick,' that dreadful white "
                                 L"whale from the High Arctic regions, to the stupendous kraken whose tentacles could entwine a 500-ton craft "
                                 L"and drag it into the ocean depths. They even reprinted reports from ancient times: the views of Aristotle and "
                                 L"Pliny accepting the existence of such monsters, then the Norwegian stories of Bishop Pontoppidan, the narratives"
                                 L" of Paul Egede, and finally the reports of Captain Harrington -- whose good faith is above suspicion--in which "
                                 L"he claims he saw, while aboard the Castilian in 1857, one of those enormous serpents that, until then, had "
                                 L"frequented only the seas of France's old extremist newspaper, The Constitutionalist.\n";

/* -------------------------------------- */

const wchar_t *pdf_20k_chapter2= L"During the period in which these developments were occurring, I had returned from a scientific"
                                 L" undertaking organized to explore the Nebraska badlands in the United States. In my capacity as "
                                 L"Assistant Professor at the Paris Museum of Natural History, I had been attached to this expedition"
                                 L" by the French government. After spending six months in Nebraska, I arrived in New York laden with "
                                 L"valuable collections near the end of March. My departure for France was set for early May. "
                                 L"In the meantime, then, I was busy classifying my mineralogical, botanical, and zoological treasures"
                                 L" when that incident took place with the Scotia.\nI was perfectly abreast of this question, which was"
                                 L" the big news of the day, and how could I not have been? I had read and reread every American and "
                                 L"European newspaper without being any farther along. This mystery puzzled me. Finding it impossible "
                                 L"to form any views, I drifted from one extreme to the other. Something was out there, that much was "
                                 L"certain, and any doubting Thomas was invited to place his finger on the Scotia's wound.\nWhen I "
                                 L"arrived in New York, the question was at the boiling point. The hypothesis of a drifting islet or "
                                 L"an elusive reef, put forward by people not quite in their right minds, was completely eliminated. And "
                                 L"indeed, unless this reef had an engine in its belly, how could it move about with such prodigious speed?\n"
                                 L"Also discredited was the idea of a floating hull or some other enormous wreckage, and again because of "
                                 L"this speed of movement.\nSo only two possible solutions to the question were left, creating two very "
                                 L"distinct groups of supporters: on one side, those favoring a monster of colossal strength; on the other"
                                 L", those favoring an 'underwater boat' of tremendous motor power.\nNow then, although the latter hypothesis "
                                 L"was completely admissible, it couldn't stand up to inquiries conducted in both the New World and the Old."
                                 L" That a private individual had such a mechanism at his disposal was less than probable. Where and when "
                                 L"had he built it, and how could he have built it in secret?\nOnly some government could own such an engine "
                                 L"of destruction, and in these disaster-filled times, when men tax their ingenuity to build increasingly "
                                 L"powerful aggressive weapons, it was possible that, unknown to the rest of the world, some nation could "
                                 L"have been testing such a fearsome machine. The Chassepot rifle led to the torpedo, and the torpedo has "
                                 L"led to this underwater battering ram, which in turn will lead to the world putting its foot down. At "
                                 L"least I hope it will.\nBut this hypothesis of a war machine collapsed in the face of formal denials from"
                                 L" the various governments. Since the public interest was at stake and transoceanic travel was suffering,"
                                 L" the sincerity of these governments could not be doubted. Besides, how could the assembly of this underwater"
                                 L" boat have escaped public notice? Keeping a secret under such circumstances would be difficult enough for "
                                 L"an individual, and certainly impossible for a nation whose every move is under constant surveillance by "
                                 L"rival powers.\nSo, after inquiries conducted in England, France, Russia, Prussia, Spain, Italy, America, "
                                 L"and even Turkey, the hypothesis of an underwater Monitor was ultimately rejected.\nAfter I arrived in New "
                                 L"York, several people did me the honor of consulting me on the phenomenon in question. In France I had "
                                 L"published a two-volume work, in quarto, entitled The Mysteries of the Great Ocean Depths. Well received"
                                 L" in scholarly circles, this book had established me as a specialist in this pretty obscure field of natural"
                                 L" history. My views were in demand. As long as I could deny the reality of the business, I confined myself to"
                                 L" a flat 'no comment.' But soon, pinned to the wall, I had to explain myself straight out. And in this vein,"
                                 L" 'the honorable Pierre Aronnax, Professor at the Paris Museum,' was summoned by The New York Herald to "
                                 L"formulate his views no matter what.\nI complied. Since I could no longer hold my tongue, I let it wag. I "
                                 L"discussed the question in its every aspect, both political and scientific, and this is an excerpt from the"
                                 L" well-padded article I published in the issue of April 30.\n\n'Therefore,' I wrote, 'after examining these "
                                 L"different hypotheses one by one, we are forced, every other supposition having been refuted, "
                                 L"to accept the existence of an extremely powerful marine animal.\n'The deepest parts of the ocean are "
                                 L"totally unknown to us. No soundings have been able to reach them. What goes on in those distant depths? "
                                 L"What creatures inhabit, or could inhabit, those regions twelve or fifteen miles beneath the surface of the "
                                 L"water? What is the constitution of these animals? It's almost beyond conjecture.\n'However, the solution"
                                 L" to this problem submitted to me can take the form of a choice between two alternatives.\n'Either we know every"
                                 L" variety of creature populating our planet, or we do not.'If we do not know every one of them, if nature still"
                                 L" keeps ichthyological secrets from us, nothing is more admissible than to accept the existence of fish or "
                                 L"cetaceans of new species or even new genera, animals with a basically 'cast-iron' constitution that inhabit "
                                 L"strata beyond the reach of our soundings, and which some development or other, an urge or a whim if you prefer,"
                                 L" can bring to the upper level of the ocean for long intervals.\n'If, on the other hand, we do know every living "
                                 L"species, we must look for the animal in question among those marine creatures already cataloged, and in this "
                                 L"event I would be inclined to accept the existence of a giant narwhale.\n'The common narwhale, or sea unicorn, "
                                 L"often reaches a length of sixty feet. Increase its dimensions fivefold or even tenfold, then give this cetacean"
                                 L" a strength in proportion to its size while enlarging its offensive weapons, and you have the animal we're "
                                 L"looking for. It would have the proportions determined by the officers of the Shannon, the instrument needed "
                                 L"to perforate the Scotia, and the power to pierce a steamer's hull.\n'In essence, the narwhale is armed with a "
                                 L"sort of ivory sword, or lance, as certain naturalists have expressed it. It's a king-sized tooth as hard as "
                                 L"steel. Some of these teeth have been found buried in the bodies of baleen whales, which the narwhale attacks "
                                 L"with invariable success. Others have been wrenched, not without difficulty, from the undersides of vessels "
                                 L"that narwhales have pierced clean through, as a gimlet pierces a wine barrel. The museum at the Faculty of "
                                 L"Medicine in Paris owns one of these tusks with a length of 2.25 meters and a width at its base of forty-eight "
                                 L"centimeters!\n'All right then! Imagine this weapon to be ten times stronger and the animal ten times more "
                                 L"powerful, launch it at a speed of twenty miles per hour, multiply its mass times its velocity, and you get "
                                 L"just the collision we need to cause the specified catastrophe.\n'So, until information becomes more abundant, "
                                 L"I plump for a sea unicorn of colossal dimensions, no longer armed with a mere lance but with an actual spur, "
                                 L"like ironclad frigates or those warships called 'rams,' whose mass and motor power it would possess "
                                 L"simultaneously.\n'This inexplicable phenomenon is thus explained away--unless it's something else entirely, "
                                 L"which, despite everything that has been sighted, studied, explored and experienced, is still possible!'\n";

/* -------------------------------------- */

int hlnk = 0;

float y_col = 0;
int   n_col = 0;

void my_pdf_header(pdf_t  *pdf)
{
    float w;
    wchar_t *title;

	pdf_set_font(pdf,"arial",PDF_FNT_BOLD,15);

    title = pdf_get_metadata(pdf,PDF_DOC_TITLE);

	w = pdf_get_text_width(pdf,title) + 6;

	pdf_set_x(pdf,(210 - w) / 2);

	pdf_set_draw_color(pdf,0,80,180);
	pdf_set_fill_color(pdf,230,230,0);
	pdf_set_text_color(pdf,220,50,50);

	pdf_set_line_width(pdf,1);
    pdf_set_line_style(pdf,PDF_SOLID_LINE);
	pdf_draw_cell(pdf,w,9,PDF_A_LINE_F|PDF_A_FULL_B|PDF_A_FILLED|PDF_A_CENTRE_J,hlnk,title);

	pdf_line_break(pdf,10);

	if(y_col == 0)
	    y_col = pdf_get_y(pdf);
}

/* -------------------------------------- */

void my_pdf_footer(pdf_t  *pdf)
{
	pdf_set_margins(pdf,10,10,10,10);
	pdf_set_y(pdf,-15,TRUE);
    pdf_set_font(pdf,"arial",PDF_FNT_ITALIC,10);
	pdf_set_text_color(pdf,128,128,128);
	pdf_draw_cell(pdf,0,0,PDF_A_CENTRE_J,0,L"Page %d of {nb}",pdf_page_number(pdf));
}

/* -------------------------------------- */

void pdf_chapter_title(pdf_t  *pdf,int number,wchar_t *title)
{
    pdf_set_font(pdf,"arial",PDF_FNT_REGULAR,12);
	pdf_set_fill_color(pdf,200,220,255);
	pdf_draw_cell(pdf,0,6,PDF_A_LINE_F|PDF_A_FILLED|PDF_A_LEFT_J,0,L"Chapter %d : %ls",number,title);

	pdf_line_break(pdf,4);

    y_col = pdf_get_y(pdf);
}

/* -------------------------------------- */

void pdf_chapter_body(pdf_t  *pdf,const wchar_t *text)
{
    pdf_set_font(pdf,"Times",PDF_FNT_REGULAR,12);
    pdf_set_cell_margin(pdf,0);
	pdf_draw_multi_cell(pdf,0,5,PDF_A_FULL_J,text);
    pdf_line_break(pdf,5);
//    pdf_set_font(pdf,NULL,PDF_FNT_ITALIC,12);
    pdf_draw_cell(pdf,0,5,PDF_A_LEFT_J,0,L"(end of excerpt)");
    pdf_line_feed(pdf);
}

/* -------------------------------------- */

void pdf_set_col(pdf_t  *pdf,int c)
{
    float x;
    n_col = c;
    x   = 10 + n_col * 65;
    pdf_set_left_margin(pdf,x);
    pdf_set_x(pdf,x);
    pdf_set_y(pdf,y_col,FALSE);
}

/* -------------------------------------- */

int my_pdf_accept_break(pdf_t  *pdf)
{
    if(n_col < 2)
    {
        pdf_set_col(pdf,n_col+1);
        return FALSE;
    }
    pdf_set_col(pdf,0);
    y_col = 0;
    return TRUE;
}


/* -------------------------------------- */

void pdf_chapter_column_body(pdf_t  *pdf,const wchar_t *text)
{
    pdf_set_font(pdf,"Times",PDF_FNT_REGULAR,12);
    pdf_set_cell_margin(pdf,0);
	pdf_draw_multi_cell(pdf,60,5,PDF_A_FULL_J,text);
    pdf_line_feed(pdf);
    pdf_draw_cell(pdf,0,5,PDF_A_LEFT_J,0,L"(end of excerpt)");
    pdf_set_col(pdf,0);
}

/* -------------------------------------- */


const wchar_t *html_text = L"You can now easily print text mixing different styles: <b>bold</b>, <i>italic</i>, "
                           L"<u>underlined</u> or <b><i><u>all at once</u></i></b>!<br><br>You can also insert links on "
                           L"the text, such as <a href=\"http://www.fpdf.org\">this one</a>, or on an image: click on the logo.";

void test_pdf(void)
{
    pdf_t *pdf;
    int ret,lnk = 0,tl1,tl2,tl3,tl4,tl5;

    printf("\n*** PDF Test ***\n\n");

    pdf = pdf_create(PDF_PORTRAIT,PDF_UNIT_MM,PDF_SIZE_A4);
    if(pdf == NULL)
    {
        printf("Cannot create PDF\n");
        return;
    }

    printf("Created PDF %s of %3.f x %3.f %s, %s\n",pdf_get_orientationc(pdf),pdf_get_width(pdf),pdf_get_height(pdf),pdf_get_unitc(pdf),pdf_get_sizec(pdf));


    pdf_set_metadata(pdf,PDF_DOC_TITLE,L"20000 Leguas de viaje submarino");
    pdf_set_metadata(pdf,PDF_DOC_AUTHOR,L"Julio Verne");

    pdf_add_page(pdf);

    pdf_draw_horz_mm_ruler(pdf,10,10,0,1,FALSE);
    pdf_draw_vert_mm_ruler(pdf,10,10,0,1,TRUE);

    pdf_write_text(pdf,0,L"Here at %.2f,%.2f",pdf->x,pdf->y);

    pdf_set_line_style(pdf,PDF_DOTTED_LINE);

    pdf_draw_ellipse(pdf,90,50,30,20,0,0);

    pdf_set_line_style(pdf,PDF_DASHED_LINE);

    pdf_set_fill_color(pdf,255,255,0);

    hlnk = pdf_add_target_link(pdf);

    pdf_draw_circle(pdf,80,47,7,PDF_A_RND_F|PDF_A_FILLED,hlnk);

    pdf_set_line_style(pdf,PDF_MORSE_LINE);

    pdf_draw_rectangle(pdf,80,147,100,100,PDF_A_FULL_B|PDF_A_FILLED|PDF_A_RND_F,0);

    pdf_set_line_style(pdf,PDF_DASHGAP_LINE);

    pdf_draw_line(pdf,80,147,80,47);

    pdf_set_line_style(pdf,PDF_SOLID_LINE);

    pdf_draw_barcode(pdf,20, 40,0,0,FALSE,L"5901234123457");
    pdf_draw_barcode(pdf,20, 70,0,0,FALSE,L"8711253001202");
    pdf_draw_barcode(pdf,25,100,0,0,FALSE,L"96385074");
    pdf_draw_barcode(pdf,20,130,0,0,FALSE,L"963859740001");
    //pdf_draw_barcode(pdf,25,130,0,FALSE,0,L"9781565924796");
    pdf_draw_barcode(pdf,25,160,0,0,FALSE,L"54495");

    pdf_draw_barcode(pdf,20,190,30,0,FALSE,L"2006061006256+78");
    pdf_draw_barcode(pdf,20,210,15,0,FALSE,L"2006061006256-78");

    pdf_draw_barcode(pdf,20,220,0,0,FALSE,L"0011499135005+02378");

    lnk = pdf_add_target_link(pdf);

    //pdf_draw_qrcode(pdf,125,160,25,0,lnk,TRUE,L"https://www.guillermoamodeo.com");

    pdf_draw_barcode(pdf,155,155,15,lnk,TRUE,L"54495");
    pdf_draw_barcode(pdf,85,160,25,lnk,TRUE,L"96385074");
    pdf_draw_barcode(pdf,165,180,0,lnk,TRUE,L"10");
    pdf_draw_barcode(pdf,85,195,0,lnk,FALSE,L"0963859740001-21");
    pdf_draw_barcode(pdf,85,220,0,lnk,TRUE,L"9781565924796-54495");

    pdf_draw_barcode(pdf,85,100,25,0,FALSE,L"54495");

    pdf_set_fill_color(pdf,0,200,255);
    pdf_set_line_style(pdf,PDF_DASHED_LINE);
    pdf_draw_rectangle(pdf,135,35,60,60,PDF_A_FULL_B|PDF_A_FILLED|PDF_A_RND_TL|PDF_A_RND_BR,lnk);
    //pdf_draw_qrcode(pdf,135,35,60,0,0,FALSE,L"s=MIWIFI_YXfF_24\np=ZudyP4YY\nc=EUwY7Sqca2_Or1\nt=EUwY7Sqca2_Or1\nl=enr=eu\n");

    //pdf_set_line_style(pdf,PDF_SOLID_LINE);

    pdf_draw_horz_mm_ruler(pdf,135,33,60,0,TRUE);
    pdf_draw_vert_mm_ruler(pdf,133,35,60,0,TRUE);

    //hlnk = pdf_add_target_link(pdf);

    pdf_set_header_function(pdf,(pdf_user_func_t)my_pdf_header);
    pdf_add_page(pdf);
    pdf_set_footer_function(pdf,(pdf_user_func_t)my_pdf_footer);
    pdf_set_page_number_alias(pdf,L"{nb}");
   	pdf_chapter_title(pdf,1,L"A RUNAWAY REEF");
   	pdf_chapter_body(pdf,pdf_20k_chapter1);
    pdf_add_page(pdf);
  	pdf_chapter_title(pdf,2,L"THE PROS AND CONS");
   	pdf_chapter_body(pdf,pdf_20k_chapter2);
    tl1 = pdf_load_image(pdf,L"things/margaret.jpg");
    if(tl1 > 0)
        printf("Image is %d\n",tl1);
    tl2 = pdf_load_image(pdf,L"things/mica.png");
    if(tl2 > 0)
        printf("Image is %d\n",tl2);

    //pdf_set_auto_page_break(pdf,FALSE);

    //pdf_go_to_page(pdf,2);

    pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,lnk,0,0);

    pdf_draw_image(pdf,-1,-1,180,224,tl1,0);
    pdf_draw_image(pdf,60,22,0,0,tl2,0);

    // Vamonos que nos vamos

    pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,hlnk,0,0);

    pdf_set_font(pdf,"arial",PDF_FNT_REGULAR,20);

    pdf_write_text(pdf,0,L"To find out what's new in this tutorial, click ");

    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_UNDERLINE);

    lnk = pdf_add_target_link(pdf);
    pdf_write_text(pdf,lnk,L"here");

    pdf_set_font_style(pdf,PDF_FNT_ITALIC);
    pdf_write_text(pdf,0,L". The text here is ");
    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_ITALIC);
    pdf_write_text(pdf,0,L"written");
    pdf_set_font_style(pdf,PDF_FNT_ITALIC);
    pdf_write_text(pdf,0,L" in flowing mode.");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" Really? hum... ...this is awkward. Well ");
    pdf_set_font_style(pdf,PDF_FNT_UNDERLINE);
    pdf_write_text(pdf,lnk,L"click me");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" to fly somewhere else!!!");

    pdf_line_feed(pdf);
    pdf_line_feed(pdf);
    pdf_set_font(pdf,"courier",PDF_FNT_REGULAR,15);
    pdf_write_text(pdf,0,L"In the 3 pages after the next one we will show tables, click ");
    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_ITALIC);
    tl1 = pdf_add_target_link(pdf);
    pdf_write_text(pdf,tl1,L"here");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" to go to the Basic Table. Or click ");
    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_ITALIC);
    tl2 = pdf_add_target_link(pdf);
    pdf_write_text(pdf,tl2,L"here");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" to go to the Improved Table. You can also see a ");
    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_UNDERLINE);
    tl3 = pdf_add_target_link(pdf);
    pdf_write_text(pdf,tl3,L"Fancy Table");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" by clicking on the underlined 'Fancy Table' text.");

    pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,lnk,0,0);

    lnk = pdf_add_target_link(pdf);

    pdf_set_target_link_as_uri(pdf,lnk,"http://www.fpdf.org");

    ret = pdf_load_image(pdf,L"things/logo.png");
    if(ret > 0)
    {
        printf("Logo image is %d\n",ret);
        pdf_draw_image(pdf,10,pdf->y,30,0,ret,lnk);
    }

    pdf_set_left_margin(pdf,45);
    pdf_set_font_size(pdf,14);
    pdf_write_html(pdf,html_text);

    pdf_set_left_margin(pdf,10);

    pdf_line_break(pdf,20);

    pdf_set_font(pdf,"Times",PDF_FNT_REGULAR,18);
    pdf_write_text(pdf,0,L" After this page you can find the previous two chapters of the book '%ls'. Click in ",pdf_get_metadata(pdf,PDF_DOC_TITLE));
    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_ITALIC|PDF_FNT_UNDERLINE);
    tl4 = pdf_add_target_link(pdf);
    pdf_write_text(pdf,tl4,L"here");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" to go to the 1st Chapter. Or ");
    pdf_set_font_style(pdf,PDF_FNT_ITALIC);
    pdf_write_text(pdf,0,L"click in ");
    pdf_set_font_style(pdf,PDF_FNT_BOLD|PDF_FNT_UNDERLINE);
    tl5 = pdf_add_target_link(pdf);
    pdf_write_text(pdf,tl5,L"here");
    pdf_set_font_style(pdf,PDF_FNT_REGULAR);
    pdf_write_text(pdf,0,L" to go to the 2nd Chapter.");

    pdf_line_break(pdf,20);

    pdf_load_font(pdf,NULL,L"things/calligra.ttf");
    pdf_write_text(pdf,0,L"This text should be written using font Calligrapher-Regular of %d points.",pdf_get_font_size(pdf));
    pdf_set_font_size(pdf,22);
    pdf_write_text(pdf,0,L" This one is written using %s of %d points,",pdf_get_font_name(pdf),pdf_get_font_size(pdf));

    pdf_set_cell_margin(pdf,1);

    pdf_add_page(pdf);

    pdf_set_target_link_as_local(pdf,tl1,0,0);
    pdf_basic_table(pdf);

    pdf_load_font(pdf,NULL,L"things/vera.ttf");
    
    pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,tl2,0,0);
    pdf_improved_table(pdf);

    pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,tl3,0,0);
    pdf_fancy_table(pdf,0);
    pdf_line_feed(pdf);
    pdf_line_feed(pdf);
    pdf_fancy_table(pdf,0.5);
    pdf_set_accept_auto_page_break(pdf,(pdf_user_func_t)my_pdf_accept_break);

   	pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,tl4,0,0);
   	pdf_chapter_title(pdf,1,L"A RUNAWAY REEF (in columns)");
   	pdf_chapter_column_body(pdf,pdf_20k_chapter1);

    pdf_add_page(pdf);
    pdf_set_target_link_as_local(pdf,tl5,0,0);
  	pdf_chapter_title(pdf,2,L"THE PROS AND CONS (in columns)");
   	pdf_chapter_column_body(pdf,pdf_20k_chapter2);    

    pdf_close(pdf);

    pdf_output(pdf,"example.pdf");

    pdf_set_compression(pdf,FALSE);
    pdf_set_links_border(pdf,TRUE);

    pdf_set_display_mode(pdf,PDF_ZOOM_FULLPAGE,PDF_PAGE_SINGLE);

    pdf_output(pdf,"example_uncompressed.pdf");

    pdf_destroy(pdf);


}

int main(void)
{
    test_pdf();
    return 0;
}

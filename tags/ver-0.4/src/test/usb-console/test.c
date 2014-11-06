/* 
 * File:	 usb-test.c
 * Author:   Robinson Mittmann (bobmittmann@gmail.com)
 * Target:
 * Comment:
 * Copyright(C) 2011 Bob Mittmann. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <sys/stm32f.h>
#include <arch/cortex-m3.h>
#include <sys/delay.h>
#include <sys/serial.h>
#include <sys/param.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <sys/dcclog.h>
#include <sys/usb-cdc.h>
#include <sys/tty.h>

#include <thinkos.h>

const char zarathustra[] =
	"1.\n\n"
	"When Zarathustra was thirty years old, he left his home and the lake of "
	"his home, and went into the mountains.  There he enjoyed his spirit and "
	"solitude, and for ten years did not weary of it.  But at last his heart "
	"changed,--and rising one morning with the rosy dawn, he went before the "
	"sun, and spake thus unto it:\n\n"
	"Thou great star!  What would be thy happiness if thou hadst not those for "
	"whom thou shinest!\n\n"
	"For ten years hast thou climbed hither unto my cave:  thou wouldst have "
	"wearied of thy light and of the journey, had it not been for me, mine "
	"eagle, and my serpent.\n\n"
	"But we awaited thee every morning, took from thee thine overflow "
	"and blessed thee for it.\n\n"
	"Lo!  I am weary of my wisdom, like the bee that hath gathered too much "
	"honey; I need hands outstretched to take it.\n\n"
	"I would fain bestow and distribute, until the wise have once more become "
	"joyous in their folly, and the poor happy in their riches.\n\n"
	"Therefore must I descend into the deep:  as thou doest in the evening, "
	"when thou goest behind the sea, and givest light "
	"also to the nether-world, "
	"thou exuberant star!\n\n"
	"Like thee must I GO DOWN, as men say, to whom I shall descend.\n\n"
	"Bless me, then, thou tranquil eye, that canst behold even the greatest "
	"happiness without envy!\n\n"
	"Bless the cup that is about to overflow, "
	"that the water may flow golden out "
	"of it, and carry everywhere the reflection of thy bliss!\n\n"
	"Lo!  This cup is again going to empty itself, and Zarathustra is again "
	"going to be a man.\n\n"
	"Thus began Zarathustra's down-going.\n\n\n"
	"2.\n\n"
	"Zarathustra went down the mountain alone, no one meeting him.  When he "
	"entered the forest, however, there suddenly stood before him an old man, "
	"who had left his holy cot to seek roots.  And thus spake the old man to "
	"Zarathustra:\n"
	"\"No stranger to me is this wanderer:  many years ago passed he by. "
	"Zarathustra he was called; but he hath altered.\n"
	"Then thou carriedst thine ashes into the mountains:  wilt thou now carry "
	"thy fire into the valleys?  Fearest thou not the incendiary's doom?\n"
	"Yea, I recognise Zarathustra.  Pure is his eye, and no loathing lurketh "
	"about his mouth.  Goeth he not along like a dancer?\n"
	"Altered is Zarathustra; a child hath Zarathustra become; "
	"an awakened one is "
	"Zarathustra:  what wilt thou do in the land of the sleepers?\n"
	"As in the sea hast thou lived in solitude, and it hath borne thee up. "
	"Alas, wilt thou now go ashore?  Alas, wilt thou again drag thy body "
	"thyself?\"\n"
	"Zarathustra answered:  \"I love mankind.\"\n"
	"\"Why,\" said the saint, \"did I go into the forest and the desert?  "
	"Was it "
	"not because I loved men far too well?\n"
	"Now I love God:  men, I do not love.  "
	"Man is a thing too imperfect for me. "
	"Love to man would be fatal to me.\"\n"
	"Zarathustra answered:  \"What spake I of love!  I am bringing gifts unto "
	"men.\"\n"
	"\"Give them nothing,\" said the saint.  "
	"\"Take rather part of their load, and "
	"carry it along with them--that will be most agreeable unto them:  if only "
	"it be agreeable unto thee!\n"
	"If, however, thou wilt give unto them, "
	"give them no more than an alms, and "
	"let them also beg for it!\"\n"
	"\"No,\" replied Zarathustra, \"I give no alms.  I am not poor enough for "
	"that.\"\n"
	"The saint laughed at Zarathustra, and spake thus:  \"Then see to it that "
	"they accept thy treasures!  They are distrustful "
	"of anchorites, and do not "
	"believe that we come with gifts.\n"
	"The fall of our footsteps ringeth too hollow through their streets.  And "
	"just as at night, when they are in bed and hear a man abroad long before "
	"sunrise, so they ask themselves concerning us:  Where goeth the thief?\n"
	"Go not to men, but stay in the forest!  "
	"Go rather to the animals!  Why not "
	"be like me--a bear amongst bears, a bird amongst birds?\"\n"
	"\"And what doeth the saint in the forest?\" asked Zarathustra.\n"
	"The saint answered:  \"I make hymns and sing them; and in making hymns "
	"I laugh and weep and mumble:  thus do I praise God.\n"
	"With singing, weeping, laughing, "
	"and mumbling do I praise the God who is my "
	"God.  But what dost thou bring us as a gift?\"\n"
	"When Zarathustra had heard these words, he bowed to the saint and said: "
	"\"What should I have to give thee!  Let me rather hurry hence lest I take "
	"aught away from thee!\"--And thus they parted "
	"from one another, the old man "
	"and Zarathustra, laughing like schoolboys.\n"
	"When Zarathustra was alone, however, he said to his heart:  \"Could it be "
	"possible!  This old saint in the forest "
	"hath not yet heard of it, that GOD "
	"IS DEAD!\"\n\n"
	"3.\n\n"
	"When Zarathustra arrived at the nearest town which "
	"adjoineth the forest, he "
	"found many people assembled in the market-place; "
	"for it had been announced "
	"that a rope-dancer would give a performance.  "
	"And Zarathustra spake thus "
	"unto the people:\n"
	"I TEACH YOU THE SUPERMAN.  "
	"Man is something that is to be surpassed.  What "
	"have ye done to surpass man?\n"
	"All beings hitherto have created something "
	"beyond themselves:  and ye want "
	"to be the ebb of that great tide, and would rather go back to the beast "
	"than surpass man?\n"
	"What is the ape to man?  A laughing-stock, "
	"a thing of shame.  And just the "
	"same shall man be to the Superman:  a laughing-stock, a thing of shame.\n"
	"Ye have made your way from the worm to man, and much within you is still "
	"worm.  Once were ye apes, and even yet man is more of an ape than any of "
	"the apes.\n"
	"Even the wisest among you is only a disharmony and hybrid of plant and "
	"phantom.  But do I bid you become phantoms or plants?\n"
	"Lo, I teach you the Superman!\n"
	"The Superman is the meaning of the earth.  "
	"Let your will say:  The Superman "
	"SHALL BE the meaning of the earth!\n"
	"I conjure you, my brethren, REMAIN TRUE TO THE EARTH, "
	"and believe not those "
	"who speak unto you of superearthly hopes!  "
	"Poisoners are they, whether they "
	"know it or not.\n"
	"Despisers of life are they, decaying ones "
	"and poisoned ones themselves, of "
	"whom the earth is weary:  so away with them!\n"
	"Once blasphemy against God was the greatest blasphemy; but God died, and "
	"therewith also those blasphemers.  To blaspheme the earth is now the "
	"dreadfulest sin, and to rate the heart of the unknowable higher than the "
	"meaning of the earth!\n"
	"Once the soul looked contemptuously on the "
	"body, and then that contempt was "
	"the supreme thing:--the soul wished the body "
	"meagre, ghastly, and famished. "
	"Thus it thought to escape from the body and the earth.\n"
	"Oh, that soul was itself meagre, ghastly, and "
	"famished; and cruelty was the "
	"delight of that soul!\n"
	"But ye, also, my brethren, tell me:  What doth your body say about your "
	"soul?  Is your soul not poverty and pollution and wretched self- "
	"complacency?\n"
	"Verily, a polluted stream is man.  "
	"One must be a sea, to receive a polluted "
	"stream without becoming impure.\n"
	"Lo, I teach you the Superman:  he is that sea; in him can your great "
	"contempt be submerged.\n"
	"What is the greatest thing ye can experience?  It is the hour of great "
	"contempt.  The hour in which even your happiness becometh loathsome unto "
	"you, and so also your reason and virtue.\n"
	"The hour when ye say:  \"What good is my happiness!  It is poverty and "
	"pollution and wretched self-complacency.  But my happiness should justify "
	"existence itself!\"\n"
	"The hour when ye say:  \"What good is my "
	"reason!  Doth it long for knowledge "
	"as the lion for his food?  It is poverty and pollution and wretched self- "
	"complacency!\"\n"
	"The hour when ye say:  \"What good is my virtue!  "
	"As yet it hath not made me "
	"passionate.  How weary I am of my good and my bad!  It is all poverty and "
	"pollution and wretched self-complacency!\"\n"
	"The hour when ye say:  \"What good is my justice!  I do not see that I am "
	"fervour and fuel.  The just, however, are fervour and fuel!\"\n"
	"The hour when we say:  \"What good is my pity!  Is not pity the cross on "
	"which he is nailed who loveth man?  But my pity is not a crucifixion.\"\n"
	"Have ye ever spoken thus?  Have ye ever cried thus?  Ah! would that I had "
	"heard you crying thus!\n"
	"It is not your sin--it is your self-satisfaction that crieth unto heaven; "
	"your very sparingness in sin crieth unto heaven!\n"
	"Where is the lightning to lick you with its tongue?  Where is the frenzy "
	"with which ye should be inoculated?\n"
	"Lo, I teach you the Superman:  "
	"he is that lightning, he is that frenzy!--\n"
	"When Zarathustra had thus spoken, one "
	"of the people called out:  \"We have "
	"now heard enough of the rope-dancer; it is time now for us to see him!\" "
	"And all the people laughed at Zarathustra.  But the rope-dancer, who "
	"thought the words applied to him, began his performance.\n\n"
	"4.\n\n"
	"Zarathustra, however, looked at the people and wondered.  Then he spake\nthus:\n"
	"Man is a rope stretched between the animal and the Superman--a rope over an "
	"abyss.\n"
	"A dangerous crossing, a dangerous wayfaring, a dangerous looking-back, a "
	"dangerous trembling and halting.\n"
	"What is great in man is that he is a bridge and not a goal:  what is "
	"lovable in man is that he is an OVER-GOING and a DOWN-GOING.\n"
	"I love those that know not how to live except as down-goers, for they are "
	"the over-goers.\n"
	"I love the great despisers, because they are the great adorers, and arrows "
	"of longing for the other shore.\n"
	"I love those who do not first seek a reason beyond the stars for going down "
	"and being sacrifices, but sacrifice themselves to the earth, that the earth "
	"of the Superman may hereafter arrive.\n"
	"I love him who liveth in order to know, and seeketh to know in order that "
	"the Superman may hereafter live.  Thus seeketh he his own down-going.\n"
	"I love him who laboureth and inventeth, that he may build the house for the "
	"Superman, and prepare for him earth, animal, and plant:  for thus seeketh "
	"he his own down-going.\n"
	"I love him who loveth his virtue:  for virtue is the will to down-going, "
	"and an arrow of longing.\n"
	"I love him who reserveth no share of spirit for himself, but wanteth to be "
	"wholly the spirit of his virtue:  thus walketh he as spirit over the "
	"bridge.\n"
	"I love him who maketh his virtue his inclination and destiny:  thus, for "
	"the sake of his virtue, he is willing to live on, or live no more.\n"
	"I love him who desireth not too many virtues.  One virtue is more of a "
	"virtue than two, because it is more of a knot for one's destiny to cling "
	"to.\n"
	"I love him whose soul is lavish, who wanteth no thanks and doth not give "
	"back:  for he always bestoweth, and desireth not to keep for himself.\n"
	"I love him who is ashamed when the dice fall in his favour, and who then "
	"asketh:  \"Am I a dishonest player?\"--for he is willing to succumb.\n"
	"I love him who scattereth golden words in advance of his deeds, and always "
	"doeth more than he promiseth:  for he seeketh his own down-going.\n"
	"I love him who justifieth the future ones, and redeemeth the past ones:  "
	"for he is willing to succumb through the present ones.\n"
	"I love him who chasteneth his God, because he loveth his God:  for he must "
	"succumb through the wrath of his God.\n"
	"I love him whose soul is deep even in the wounding, and may succumb through "
	"a small matter:  thus goeth he willingly over the bridge.\n"
	"I love him whose soul is so overfull that he forgetteth himself, and all "
	"things are in him:  thus all things become his down-going.\n"
	"I love him who is of a free spirit and a free heart:  thus is his head only "
	"the bowels of his heart; his heart, however, causeth his down-going.\n"
	"I love all who are like heavy drops falling one by one out of the dark "
	"cloud that lowereth over man:  they herald the coming of the lightning, and "
	"succumb as heralds.\n"
	"Lo, I am a herald of the lightning, and a heavy drop out of the cloud:  the "
	"lightning, however, is the SUPERMAN.--\n\n"
	"5.\n\n"
	"When Zarathustra had spoken these words, he again looked at the people, and "
	"was silent.  \"There they stand,\" said he to his heart; \"there they laugh:  "
	"they understand me not; I am not the mouth for these ears.\n"
	"Must one first batter their ears, that they may learn to hear with their "
	"eyes?  Must one clatter like kettledrums and penitential preachers?  Or do "
	"they only believe the stammerer?\n"
	"They have something whereof they are proud.  What do they call it, that "
	"which maketh them proud?  Culture, they call it; it distinguisheth them "
	"from the goatherds.\n"
	"They dislike, therefore, to hear of 'contempt' of themselves.  So I will "
	"appeal to their pride.\n"
	"I will speak unto them of the most contemptible thing:  that, however, is "
	"THE LAST MAN!\"\n"
	"And thus spake Zarathustra unto the people:\n"
	"It is time for man to fix his goal.  It is time for man to plant the germ "
	"of his highest hope.\n"
	"Still is his soil rich enough for it.  But that soil will one day be poor "
	"and exhausted, and no lofty tree will any longer be able to grow thereon.\n"
	"Alas! there cometh the time when man will no longer launch the arrow of his "
	"longing beyond man--and the string of his bow will have unlearned to whizz!\n"
	"I tell you:  one must still have chaos in one, to give birth to a dancing "
	"star.  I tell you:  ye have still chaos in you.\n"
	"Alas!  There cometh the time when man will no longer give birth to any "
	"star.  Alas!  There cometh the time of the most despicable man, who can no "
	"longer despise himself.\n"
	"Lo!  I show you THE LAST MAN.\n"
	"\"What is love?  What is creation?  What is longing?  What is a star?\"--so "
	"asketh the last man and blinketh.\n"
	"The earth hath then become small, and on it there hoppeth the last man who "
	"maketh everything small.  His species is ineradicable like that of the "
	"ground-flea; the last man liveth longest.\n"
	"\"We have discovered happiness\"--say the last men, and blink thereby.\n"
	"They have left the regions where it is hard to live; for they need warmth.  "
	"One still loveth one's neighbour and rubbeth against him; for one needeth "
	"warmth.\n"
	"Turning ill and being distrustful, they consider sinful:  they walk warily.  "
	"He is a fool who still stumbleth over stones or men!\n"
	"A little poison now and then:  that maketh pleasant dreams.  And much "
	"poison at last for a pleasant death.\n"
	"One still worketh, for work is a pastime.  But one is careful lest the "
	"pastime should hurt one.\n"
	"One no longer becometh poor or rich; both are too burdensome.  Who still "
	"wanteth to rule?  Who still wanteth to obey?  Both are too burdensome.\n"
	"No shepherd, and one herd!  Every one wanteth the same; every one is equal:  "
	"he who hath other sentiments goeth voluntarily into the madhouse.\n"
	"\"Formerly all the world was insane,\"--say the subtlest of them, and blink "
	"thereby.\n"
	"They are clever and know all that hath happened:  so there is no end to "
	"their raillery.  People still fall out, but are soon reconciled--otherwise "
	"it spoileth their stomachs.\n"
	"They have their little pleasures for the day, and their little pleasures "
	"for the night, but they have a regard for health.\n"
	"\"We have discovered happiness,\"--say the last men, and blink thereby.--\n"
	"And here ended the first discourse of Zarathustra, which is also called "
	"\"The Prologue\":  for at this point the shouting and mirth of the multitude "
	"interrupted him.  \"Give us this last man, O Zarathustra,\"--they called out- "
	"-\"make us into these last men!  Then will we make thee a present of the "
	"Superman!\"  And all the people exulted and smacked their lips.  "
	"Zarathustra, however, turned sad, and said to his heart:\n"
	"\"They understand me not:  I am not the mouth for these ears.\n"
	"Too long, perhaps, have I lived in the mountains; too much have I hearkened "
	"unto the brooks and trees:  now do I speak unto them as unto the goatherds.\n"
	"Calm is my soul, and clear, like the mountains in the morning.  But they "
	"think me cold, and a mocker with terrible jests.\n"
	"And now do they look at me and laugh:  and while they laugh they hate me "
	"too.  There is ice in their laughter.\"\n\n";

const char msg[] = "1.\r\n\r\n"
	"When Zarathustra was thirty years old, he left his home and the lake of "
	"his home, and went into the mountains.  There he enjoyed his spirit and "
	"solitude, and for ten years did not weary of it.  But at last his heart "
	"changed,--and rising one morning with the rosy dawn, he went before the "
	"sun, and spake thus unto it:\r\n\r\n"
	"Thou great star!  What would be thy happiness if thou hadst not those for "
	"whom thou shinest!\r\n\r\n"
	"For ten years hast thou climbed hither unto my cave:  thou wouldst have "
	"wearied of thy light and of the journey, had it not been for me, mine "
	"eagle, and my serpent.\r\n\r\n"
	"But we awaited thee every morning, took from thee thine overflow "
	"and blessed thee for it.\r\n\r\n"
	"Lo!  I am weary of my wisdom, like the bee that hath gathered too much "
	"honey; I need hands outstretched to take it.\r\n\r\n"
	"I would fain bestow and distribute, until the wise have once more become "
	"joyous in their folly, and the poor happy in their riches.\r\n\r\n"
	"Therefore must I descend into the deep:  as thou doest in the evening, "
	"when thou goest behind the sea, and givest light "
	"also to the nether-world, "
	"thou exuberant star!\r\n\r\n"
	"Like thee must I GO DOWN, as men say, to whom I shall descend.\r\n\r\n"
	"Bless me, then, thou tranquil eye, that canst behold even the greatest "
	"happiness without envy!\r\n\r\n"
	"Bless the cup that is about to overflow, "
	"that the water may flow golden out "
	"of it, and carry everywhere the reflection of thy bliss!\r\n\r\n"
	"Lo!  This cup is again going to empty itself, and Zarathustra is again "
	"going to be a man.\r\n\r\n"
	"Thus began Zarathustra's down-going.\r\n\r\n\r\n";


int printer_task(FILE * f)
{
	unsigned int n;
	unsigned int sz;
	unsigned int rm;
	char * cp;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	thinkos_sleep(100);

	return 0;

	cp = (char *)zarathustra;
	sz = sizeof(zarathustra);
	rm = sz;
	while (1) {
		thinkos_sleep(20);
		n = rand() & 0x1ff;
		n = MIN(n , rm);
		fwrite(cp, n, 1, f);
		cp += n;
		rm -= n;

		if (rm == 0) {
			DCC_LOG1(LOG_TRACE, "[%d] restarting.", thinkos_thread_self());
			thinkos_sleep(2000);
			rm = sz;
			cp = (char *)zarathustra;
		}
	}

	return 0;
}

#define BUF_SIZE 256

int echo_task(FILE * f)
{
	char buf[BUF_SIZE];
	unsigned int n;

	DCC_LOG1(LOG_TRACE, "[%d] started.", thinkos_thread_self());
	thinkos_sleep(100);

	while (1) {
		n = rand() & 0x1ff;
		n = MIN(n , BUF_SIZE);
		n = fread(buf, n, 1, f);
		fwrite(buf, n, 1, f);
	}

	return 0;
}

#define STACK_SIZE 512
uint32_t printer_stack1[STACK_SIZE / 4];
uint32_t printer_stack2[STACK_SIZE / 4];

uint32_t echo_stack1[STACK_SIZE / 4];
uint32_t echo_stack2[STACK_SIZE / 4];

int main(int argc, char ** argv)
{
	usb_cdc_class_t * cdc;
	uint8_t buf[128];
	FILE * f_tty;
	FILE * f_raw;
	struct tty_dev * tty;
	int i;
	int n;

	DCC_LOG_INIT();
	DCC_LOG_CONNECT();

	DCC_LOG(LOG_TRACE, "1. cm3_udelay_calibrate()");
	cm3_udelay_calibrate();
	DCC_LOG(LOG_TRACE, "2. thinkos_init()");
	thinkos_init(THINKOS_OPT_PRIORITY(8) | THINKOS_OPT_ID(7));

	DCC_LOG(LOG_TRACE, "usb_cdc_init()");
	cdc = usb_cdc_init(&stm32f_otg_fs_dev, *((uint64_t *)STM32F_UID));

	while (0) {
		thinkos_sleep(5000);
		DCC_LOG(LOG_TRACE, "usb_cdc_write()");
		usb_cdc_write(cdc, "Hello world!", 12);
	};

	while (0) {
		DCC_LOG(LOG_TRACE, "usb_cdc_read()");
		n = usb_cdc_read(cdc, buf, 128, 0);
		(void)n;
		DCC_LOG1(LOG_TRACE, "n=%d", n);
	};

	thinkos_sleep(1000);

	usb_cdc_write(cdc, msg, sizeof(msg));

	thinkos_sleep(500);

	DCC_LOG(LOG_TRACE, "usb_cdc_fopen()");
	f_raw = usb_cdc_fopen(cdc);

	tty = tty_init(f_raw);

	f_tty = tty_fopen(tty);

	thinkos_sleep(1000);

	fwrite(zarathustra, sizeof(zarathustra), 1, f_tty);

	thinkos_sleep(1000);

#if 0
	/* create some printer threads */
	thinkos_thread_create((void *)printer_task, (void *)f, 
						  printer_stack1, STACK_SIZE, 0);

	thinkos_thread_create((void *)printer_task, (void *)f, 
						  printer_stack2, STACK_SIZE, 0);


	/* create some echo threads */
	thinkos_thread_create((void *)echo_task, (void *)f, 
						  echo_stack1, STACK_SIZE, 0);

//	thinkos_thread_create((void *)echo_task, (void *)f, 
//						  echo_stack2, STACK_SIZE, 0);
#endif


	for (i = 0; ;i++) {
		thinkos_sleep(2000);
		fprintf(f_tty, "[%d] hello world...\n", i);
	}

	return 0;
}


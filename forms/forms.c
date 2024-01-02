#include "forms.h"

/*
    выравнивает форму f относительно формы p - по сути считает положение формы f, размер формы f уже должен быть задан
    form_t * pf         родительская форма внутри которой происходит выравниаение
    form_t * af         выравниваемая форма
    align_mode_t ax     режим выравнивания по двум осям включая отступ
*/
void form_align(form_t * pf, form_t * af, align_mode_t * mode)
{
    for (unsigned i = 0; i < 2; i++) {
        switch (mode->ca[i].mode) {
        case ALIGN_LEFT_OUTSIDE:
            af->p.ca[i] = pf->p.ca[i] - mode->ca[i].offset - af->s.ca[i];
            break;

        case ALIGN_LEFT_INSIDE:
            af->p.ca[i] = pf->p.ca[i] + mode->ca[i].offset;
            break;

        case ALIGN_CENTER:
            af->p.ca[i] = pf->p.ca[i] + ((pf->s.ca[i] - af->s.ca[i]) / 2);
            break;

        case ALIGN_RIGHT_INSIDE:
            af->p.ca[i] = pf->p.ca[i] + pf->s.ca[i] - mode->ca[i].offset - af->s.ca[i];
            break;

        case ALIGN_RIGHT_OUTSIDE:
            af->p.ca[i] = pf->p.ca[i] + pf->s.ca[i] + mode->ca[i].offset;
            break;

        default:
            break;
        }
    }
}

/*
    считает итоговый размер 2х форм
    form_t * f1         первая форма
    form_t * f2         вторая форма
    align_mode_t ax     режим выравнивания по двум осям включая отступ
    form_t * rf         результирующая форма для которой будет посчитан размер
*/
void form_union_calc_size(form_t * f1, form_t * f2, align_mode_t * mode, form_t * of)
{
        for (unsigned i = 0; i < 2; i++) {
        switch (mode->ca[i].mode) {
        case ALIGN_LEFT_OUTSIDE:
        case ALIGN_RIGHT_OUTSIDE:
            of->s.ca[i] = f1->s.ca[i] + f2->s.ca[i] + mode->ca[i].offset;
            break;

        case ALIGN_LEFT_INSIDE:
        case ALIGN_CENTER:
        case ALIGN_RIGHT_INSIDE:
            if (f1->s.ca[i] > f2->s.ca[i]) {
                of->s.ca[i] = f1->s.ca[i];
            } else {
                of->s.ca[i] = f2->s.ca[i];
            }

        default:
            break;
        }
    }
}

void form_union_calc_pos(form_t * of, form_t * f1, form_t * f2, align_mode_t * mode)
{
    for (unsigned i = 0; i < 2; i++) {
        switch (mode->ca[i].mode) {
        case ALIGN_LEFT_OUTSIDE:
            // f2 слева, f1 снаружи справа
            f2->p.ca[i] = of->p.ca[i];
            f1->p.ca[i] = of->p.ca[i] + f2->s.ca[i] + mode->ca[i].offset;
            break;

        case ALIGN_LEFT_INSIDE:
            // f1 слева, f2 внутри слева
            f1->p.ca[i] = of->p.ca[i];
            f2->p.ca[i] = of->p.ca[i] + mode->ca[i].offset;
            break;

        case ALIGN_CENTER:
            // f1 по центру, f2 тоже по центру
            f1->p.ca[i] = of->p.ca[i] + ((of->s.ca[i] - f1->s.ca[i]) / 2);
            f2->p.ca[i] = of->p.ca[i] + ((of->s.ca[i] - f2->s.ca[i]) / 2);
            break;

        case ALIGN_RIGHT_INSIDE:
            f1->p.ca[i] = of->p.ca[i];
            f2->p.ca[i] = of->p.ca[i] + of->s.ca[i] - f2->s.ca[i] - mode->ca[i].offset;

        case ALIGN_RIGHT_OUTSIDE:
            // f1 слева, f2  справа
            f1->p.ca[i] = of->p.ca[i];
            f2->p.ca[i] = of->p.ca[i] + f1->s.ca[i] + mode->ca[i].offset;
            break;

        default:
            break;
        }
    }
}
#include "forms.h"

/*
    выравнивает форму f относительно формы p - по сути считает положение формы f, размер формы f уже должен быть задан
    form_t * pf         родительская форма внутри которой происходит выравниаение
    form_t * af         выравниваемая форма
    align_mode_t ax     режим выравнивания
    uint8_t offset      отступ если выравнивание происходит по краю
*/
void form_align(form_t * pf, form_t * af, align_mode_t * mode, unsigned offset)
{
    for (unsigned i = 0; i < 2; i++) {
        switch (mode->ca[i]) {
        case ALIGN_LEFT_OUTSIDE:
            af->p.ca[i] = pf->p.ca[i] - offset - af->s.ca[i];
            break;

        case ALIGN_LEFT_INSIDE:
            af->p.ca[i] = pf->p.ca[i] + offset;
            break;

        case ALIGN_CENTER:
            af->p.ca[i] = pf->p.ca[i] + ((pf->s.ca[i] - af->s.ca[i]) / 2);
            break;

        case ALIGN_RIGHT_INSIDE:
            af->p.ca[i] = pf->p.ca[i] + pf->s.ca[i] - offset - af->s.ca[i];
            break;

        case ALIGN_RIGHT_OUTSIDE:
            af->p.ca[i] = pf->p.ca[i] + pf->s.ca[i] + offset;
            break;

        default:
            break;
        }
    }
}

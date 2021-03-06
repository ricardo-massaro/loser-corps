/* cannon.c
 *
 * This was originally written for the server by Guto
 * (ragm@linux.ime.usp.br), and later adapted to work as an enemy
 * library by Moe (massaro@ime.usp.br).
 */

#include <stdio.h>

#include <../server/server.h>


#define WEAPON_DELAY  10

/* The size of the field of vision: */
#define X_VISION      (5 * (server)->tile_w)
#define Y_VISION      (2 * (server)->tile_w)



/* Shoot */
static void enemy_shoot(ENEMY *e)
{
  if(e->weapon_delay <= 0)
    {
      e->weapon = 1;
      e->weapon_delay = WEAPON_DELAY;
    } else
      e->weapon_delay--;
}

static int is_in_field(SERVER *server,
		       int jack_x, int jack_y, int enemy_x, int enemy_y)
{
  return(jack_x <= enemy_x + X_VISION &&
	 jack_x >= enemy_x - X_VISION &&
	 jack_y <= enemy_y + Y_VISION &&
	 jack_y >= enemy_y - Y_VISION);
}


static int verify_target(SERVER *server, ENEMY *e, int target)
{
  CLIENT *c;

  for(c = server->client; c != NULL && c->jack.id != target; c= c-> next);
  if(c != NULL && c->jack.invisible < 0
     && is_in_field(server, c->jack.x,c->jack.y,e->x,e->y)) return(target);
  else return(-1);
}

/* Verify presence of targets */
static int search_target(SERVER *server, ENEMY *e)
{
  CLIENT *c;

  for(c = server->client; c != NULL; c = c->next)
    if(c->jack.invisible < 0
       && is_in_field(server, c->jack.x,c->jack.y,e->x,e->y))
      return(c->jack.id);
  return(-1);
}

static void enemy_move_continue(ENEMY *e, SERVER *server, NPC_INFO *npc)
{
  int flags, move_dx, move_dy;

  if (e->state == STATE_WALK || e->state == STATE_STAND) 
  {                          /* Check the floor under enemy */
    calc_movement(server, 0, e->x, e->y, npc->clip_w, npc->clip_h,
		  0, 1, &move_dx, &move_dy);
    if (move_dy > 0)
      e->state = STATE_JUMP_END;    /* Start falling */
  }
  else
    e->dy += DEC_JUMP_SPEED;

  {
    int s;

    if (e->dx < 0) {
      e->dx = -e->dx;
      s = -1;
    } else
      s = 1;
    e->dx -= 1000;
    if (e->dx < 0)
      e->dx = 0;
    else
      e->dx *= s;
  }

  flags = calc_movement(server, 1, e->x, e->y, npc->clip_w, npc->clip_h,
			e->dx / 1000, e->dy / 1000, 
			&move_dx, &move_dy);

  if(flags & CM_Y_CLIPPED)
  {
    if(e->dy > 0)               /* Hit the ground */
    {
      e->state = STATE_STAND;
      e->dy = 0;
    }
    else                        /* Hit the ceiling */
      e->dy = -e->dy;
  }

  if(flags & CM_X_CLIPPED)
    if(e->state == STATE_WALK) e->dx *= -1;

  e->x += move_dx;
  e->y += move_dy;

  switch(e->state)
  {
  case STATE_JUMP_START:
    if(e->dy > 0) e->state = STATE_JUMP_END;
    break;

  case STATE_WALK:
    if(e->dx > 0) e->dir = DIR_RIGHT;
    else e->dir = DIR_LEFT;
  }
}


/* Do cannon enemy movement */
void move_enemy(SERVER *server, ENEMY *e, NPC_INFO *npc)
{
  CLIENT *c;

  if (server->change_map)
    return;

  e->frame++;

  /* If destroyed, leave an item behind */
  if (e->destroy) {
    ITEM *it;
    it = make_new_item(server, get_item_npc(server, "energy"),
		       e->x + npc->clip_w / 2, e->y + npc->clip_h / 2);
    if (it != NULL) {
      it->x -= server->npc_info[it->npc].clip_w / 2;
      it->y -= server->npc_info[it->npc].clip_h / 2;
    }
    return;
  }

  enemy_move_continue(e,server,npc);

  if(e->target_id >= 0)
  {
    for(c = server->client; c!=NULL && c->jack.id!=e->target_id; 
	c = c->next);

    if(c != NULL)
    {
      if(c->jack.x >= e->x) e->dir = DIR_RIGHT;
      else e->dir = DIR_LEFT;
    }
    enemy_shoot(e);
    e->target_id = verify_target(server,e,e->target_id);
  }
  else
    e->target_id = search_target(server,e);
}

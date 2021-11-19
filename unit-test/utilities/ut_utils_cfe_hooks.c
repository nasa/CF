#include <string.h>

/* UT includes */
#include "ut_utils_cfe_hooks.h"

/* NOTE: This was designed and used before the advent of the ut-assert hander
** ability.  It's use is only for the legacy code that needed it before the
** changes to ut-assert.  Handlers should be used instead of this function */

/*****************************************************************************/
/**
 * \brief Turns a standard ut-assert stub into a unit test reporter.
 *
 * \par Description
 *          This is a ut-assert hook function and follows the footprint 
 *          requirements. When used with the 'set hook' functions, <TODO:find those and enter here?>
 *          this function will record the arguments received by that stub into
 *          the provided UserObj.  (Shout out to Keegan Moore/GSFC582 for his help
 *          in the stub multi-call functionality)
 * 
 * \par Assumptions
 *          Any stub set with this hook registers the context for each argument
 *          it is given with the ut-assert framework.
 *          UserObj must be a type that is made to hold the full amount of the
 *          expected argument space.  
 * 
 * \par Example of a viable UserObj when va == NULL
 *          If this example is followed it should
 *          always be able to hold the data from a stub call that does NOT use
 *          va (variable arguments, the stuff given to '...').
 *          PLEASE NOTE: in its present state this function WILL work for any
 *          va functions, but only because they will be COMPLETELY IGNORED and
 *          any UserObj will need to reflect this if used at present.
 * 
 *          Example UserObj for CFE_EVS_SendEvent:
 *          typedef struct {
 *              uint16 EventID;
 *              uint16 EventType;
 *              const char *Spec;
 *          } __attribute__((packed)) CFE_EVS_SendEvent_context_t;
 * 
 *          When using this example and only 1 stub call is expected send the 
 *          'set hook' function the address of your context object:
 * 
 *    CFE_EVS_SendEvent_context_t   context_CFE_EVS_SendEvent;
 *    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);
 * 
 *          Note the '&', which is sending the ADDRESS of the context.
 *          However, when multiple calls are expected, an ARRAY of contexts are
 *          required, like this:
 *
 *    CFE_EVS_SendEvent_context_t   context_CFE_EVS_SendEvent[2];
 *    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, context_CFE_EVS_SendEvent);
 * 
 *          Note the removal of '&' because the name is now an ARRAY, thus 
 *          already is a pointer.
 *
 *          Stubs that use variable arguments (va) are not yet fully considered here, DO NOT USE YET
 *
 * \param[in, out]  UserObj       A pointer to a context object 
 *                                for the stub being called. Arguments'
 *                                values will be saved in here.  UserObj
 *                                must follow guidance in Assumptions
 * \param[in]       StubRetcode   The value that will be returned
 *                                by the stub call (ignored by this function)
 * \param[in]       CallCount     Number of times the stub has been called
 *                                before this time
 * \param[in]       Context       A pointer to the meta data object and
 *                                values of the arguments to be saved
 * \param[in]       va            A va_list of all variable objects sent
 *                                to the stub
 *
 * \return 0, always success because this is only for testing - test will fail
 *         when setup has been done incorrectly
 * 
 * \par int32 stub_reporter_va_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context,
 *                                va_list va)
 */
int32 stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
  uint8  i = 0;                /* i is index */
  uint8  size_used = 0;        /* determines size of argument to be saved */
  const void*  val_location = NULL;  /* pointer to arg value to be saved */
  uint8* obj_ptr;              /* tracker indicates where to push data into UserObj */

  /* Determine where in the UserObj we should be located dependent upon CallCount */
  if (CallCount == 0)
  {
      obj_ptr = (uint8*)UserObj; 
  }
  else
  {
      uint8 context_size = 0;

      for(i = 0; i < Context->ArgCount; ++i)
      {
          /* A UT_STUBCONTEXT_ARG_TYPE_DIRECT type indicates the arg itself is the ptr argument, add a (void*) size */
          if (Context->Meta[i].Type == UT_STUBCONTEXT_ARG_TYPE_DIRECT)
          {
            context_size += sizeof(void*);
          }
          else /* UT_STUBCONTEXT_ARG_TYPE_INDIRECT indicates the arg is pointing to the value to be saved, add its size */
          {
            context_size += Context->Meta[i].Size;
          } 
      }

      /* obj_ptr moves a full context_size for every call (initial value is 0) -- user object for calls > 1 must be an array of contexts */
      obj_ptr = (uint8*)UserObj + (context_size * CallCount);
  }
  
  for(i = 0; i < Context->ArgCount; ++i)
  {
    /* UT_STUBCONTEXT_ARG_TYPE_DIRECT indicates the arg is the ptr that is to be saved */
    if (Context->Meta[i].Type == UT_STUBCONTEXT_ARG_TYPE_DIRECT)
    {
      val_location = &Context->ArgPtr[i];
      size_used = sizeof(void*);
    }
    else /* UT_STUBCONTEXT_ARG_TYPE_INDIRECT indicates the arg is pointing to the value to be saved */
    {
      val_location = Context->ArgPtr[i];
      size_used = Context->Meta[i].Size;
    }
    /* put the argument value into the user object */
    memcpy(obj_ptr, val_location, size_used);
    /* move to end of this size item in the user object */
    obj_ptr += size_used;
  }
  
  // }
  return StubRetcode;
}

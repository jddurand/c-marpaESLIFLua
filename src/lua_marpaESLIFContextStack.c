static const char *MARPAESLIFLUA_CONTEXTSTACK =
  "\n"
  "-- Context stack is an array - it is assumed that the caller takes care of table.pack/table.unpack so that a context is ONE SINGLE DATA\n"
  "\n"
  "local marpaESLIFContextStack = {}\n"
  "marpaESLIFContextStack.new = function()\n"
  "  local stack  = {}                -- Initial context stack\n"
  "  local self = {}                  -- The context stack object\n"
  "\n"
  "  function self:get()\n"
  "    local c = stack[#stack]\n"
  "    -- print('Current context from stack: '..tableDump(c))\n"
  "    return c\n"
  "  end\n"
  "\n"
  "  function self:set(item)\n"
  "    stack[#stack] = item\n"
  "  end\n"
  "\n"
  "  function self:push(item)\n"
  "    stack[#stack+1] = item\n"
  "    -- print('Stack after push: '..tableDump(stack))\n"
  "  end\n"
  "\n"
  "  function self:pop()\n"
  "    stack[#stack] = nil            -- Returns nothing\n"
  "    -- print('Stack after pop: '..tableDump(stack))\n"
  "  end\n"
  "\n"
  "  return self\n"
  "end\n"
  "\n"
  "return marpaESLIFContextStack\n"
  "\n";
